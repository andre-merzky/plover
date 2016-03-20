
#include <stdlib.h>      // for ::getenv()
#include <unistd.h>      // for ::getpid()
#include <sys/types.h>   // for ::getpid() / pid_t

#include <saga/util/utils.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/scoped_lock.hpp>

// #define SAGA_UTIL_LOGGING_SEVERITY_DEFAULT saga::util::logging::Warning;
#define SAGA_UTIL_LOGGING_SEVERITY_DEFAULT saga::util::logging::Noise;
// #define SAGA_UTIL_LOGGING_TAGS_DEFAULT     "+must,-mustnot,may" // FIXME: should be empty by default
#define SAGA_UTIL_LOGGING_TAGS_DEFAULT     "*"
#define SAGA_UTIL_LOGGING_TARGET_DEFAULT   "/tmp/saga_simple.%p.%t.log"

namespace saga
{
  namespace util
  {
    // static class member initialization
    logging::logging (void)
      : per_thread_  (false)
      , per_process_ (false)
      , tags_star_   (false)
      , serial_      (0)
    {
      // getenv, getpid, open files, ...

      ////////////////////////////////////////////////////////////////
      //
      // set severity filter
      //
      severity_ = SAGA_UTIL_LOGGING_SEVERITY_DEFAULT;

      const char * severity_tmp = ::getenv ("SAGA_LOG_SEVERITY");
      if ( NULL != severity_tmp )
      {
        std::string tmp (severity_tmp);

        try 
        {
          severity_ = saga::util::saga_enums::get_singleton()->to_val <severity> (tmp.c_str (), true);
        }
        catch ( ... )
        {
          // ignore invalid value
          std::cerr << "WARNING: invalid severity setting ignored: " << tmp << std::endl;
          severity_ = SAGA_UTIL_LOGGING_SEVERITY_DEFAULT;
        }
      }

      ////////////////////////////////////////////////////////////////

      ////////////////////////////////////////////////////////////////
      //
      // set tag filter
      //
      std::string tags_str = SAGA_UTIL_LOGGING_TAGS_DEFAULT;

      const char * tags_tmp = ::getenv ("SAGA_LOG_TAGS");
      if ( NULL != tags_tmp )
      {
        tags_str = tags_tmp;
      }

      std::vector <std::string> all_tags = saga::util::split (tags_str, ", ");

      for ( unsigned int i = 0; i < all_tags.size (); i++ )
      {
        std::string tag = all_tags[i];

        // note that the substring operations below MAY result in empty tags.
        if ( ! tag.empty () )
        {
          if ( tag == "*" )
          {
            tags_star_ = true;
          }
          else if ( tag.size () > 1 && tag[0] == '+' )
          {
            tags_must_.push_back (tag.substr (1));
          }
          else if ( tag.size () > 1 && tag[0] == '-' )
          {
            tags_must_not_.push_back (tag.substr (1));
          }
          else 
          {
            tags_may_.push_back (tag);
          }
        }
      }
      ////////////////////////////////////////////////////////////////

      ////////////////////////////////////////////////////////////////
      //
      // set output targets
      //
      spec_ = SAGA_UTIL_LOGGING_TARGET_DEFAULT;

      const char * spec_tmp = ::getenv ("SAGA_LOG_TARGET");
      if ( NULL != spec_tmp )
      {
        spec_ = spec_tmp;
      }

      if ( saga::util::find ("\%t", spec_) )
      {
        per_thread_ = true;
      }

      if ( saga::util::find ("\%p", spec_) )
      {
        per_process_ = true;
      }

      {
        // we always open a stream to /dev/null, for not logging anything
        nostream_ = new std::fstream ("/dev/null", std::ios_base::out);
        
        if ( ! nostream_->good () )
        {
          std::cerr << "cannot open log stream to /dev/null "
                    << " -- using stderr" << std::endl;
          nostream_ = &std::cerr;
        }
      }
    }


    ////////////////////////////////////////////////////////////////////
    //
    //
    //
    logging::~logging (void)
    {
      std::map <pthread_t, std::ostream * > :: iterator it; 

      for ( it = streams_.begin (); it != streams_.end (); it++ )
      {
        if ( it->second != &std::cout &&
             it->second != &std::cerr )
        {
          std::cerr << "log stream closed" << std::endl;
          delete (*it).second;
        }
        else
        {
          std::cerr << "log stream !closed" << std::endl;
        }
      }

      delete nostream_;
    }


    ////////////////////////////////////////////////////////////////////
    //
    //
    //
    std::ostream & logging::logstr (severity    s,   // severity level of log
                                    std::string t)   // tags for log
    {
      if ( matches_severity (s) &&
           matches_tags     (t) )
      {
        // by default, use stream 0
        pthread_t tid = 0;

        // if per_thread logging is enabled, use the stream for this thread id (!0)
        if ( per_thread_ )
        {
          tid = pthread_self ();
        }

        // do we have a valid ostream?
        if ( streams_.find (tid) == streams_.end () )
        {
          // need to open a new stream for this thread.  
          // TODO: needs locking
          if ( "stdio"     == spec_ ||
               "std"       == spec_ ||
               "stdout"    == spec_ ||
               "std::cout" == spec_ ||
               "cout"      == spec_ ||
               "out"       == spec_ )
          {
            std::cerr << "log stream uses cout" << std::endl;
            streams_[tid] = &std::cout;
          }
          else if ( "stdio"     == spec_ ||
                    "std"       == spec_ ||
                    "stderr"    == spec_ ||
                    "std::cerr" == spec_ ||
                    "cerr"      == spec_ ||
                    "err"       == spec_ )
          {
            std::cerr << "log stream uses cerr" << std::endl;
            streams_[tid] = &std::cerr;
          }
          else
          {
            // spec is assumed to be a file name, fopen it
            std::string spec (spec_);

            size_t pos = spec.find ("\%t");
            if ( std::string::npos != pos )
            {
              spec.replace (pos, 2, saga::util::itoa (tid));
            }

            pos = spec.find ("\%p");
            if ( std::string::npos != pos )
            {
              spec.replace (pos, 2, saga::util::itoa (::getpid ()));
            }

            streams_[tid] = new std::fstream (spec.c_str (), std::ios_base::out);

            if ( ! streams_[tid]->good () )
            {
              std::cerr << "cannot open log stream for " << spec 
                        << " -- using stderr"            << std::endl;
              streams_[tid] = &std::cerr;
            }
            else
            {
              // std::cerr << "opened log stream for " << spec << std::endl;
            }
          }
        }

        // we are now sure to have a valid stream
        //
        // but before we return it, we log the log header, so that the
        // receipient can just go ahead and log the log messages...
        //
        char head[256];
        snprintf (head, 255, " %-6lld : %-10s : %-30s : ", serial_,
                  saga::util::saga_enums::get_singleton ()->to_key (s).c_str (),
                  t.c_str ());

        (*streams_[tid]) << head;

        serial_++;

        // now we can return the stream, to be used for the log message
        return (*streams_[tid]);
      }
      else
      {
        // std::cout << "logging to /dev/null" << std::endl;
        // no logging required - stream to /dev/null
        return (*nostream_);
      }
    }


    ////////////////////////////////////////////////////////////////////
    //
    //
    //
    void logging::log (severity    s,   // severity level of log
                       std::string t,   // tags for log
                       std::string m)   // log message
    { 
      logstr (s, t) << "logging: " << m << std::endl;
    }

    ////////////////////////////////////////////////////////////////////
    //
    //
    //
    bool logging::matches_severity (severity s)
    {
      if ( s < severity_ )
      {
        return false;
      }

      return true;
    }

    ////////////////////////////////////////////////////////////////////
    //
    // the check returns true if the given tags
    //
    //   - match *all*  of the tags_must_      AND
    //   - match *none* of the tags_must_not_  AND
    //   - match *any*  of the tags_may_      
    //
    // tags_must_not_ supercedes tags_must_.
    //
    bool logging::matches_tags (std::string t)
    {
      // we log all messages if '*' is part of requested tags
      if ( tags_star_ )
        return true;


      bool must_match = false;
      std::vector <std::string> tags = saga::util::split (t, ", ");

      // make sure we match all tags_must_
      for ( unsigned int i = 0; i < tags_must_.size (); i++ )
      {
        bool found = false;

        for ( unsigned int j = 0; j < tags.size (); j++ )
        {
          if ( tags_must_[i] == tags[j] )
          {
            found      = true;
            must_match = true;
            j = tags.size ();
          }
        }

        if ( ! found )
        {
          return false;
        }
      }


      // make sure we match none of tags_must_not_
      for ( unsigned int i = 0; i < tags_must_not_.size (); i++ )
      {
        for ( unsigned int j = 0; j < tags.size (); j++ )
        {
          if ( tags_must_not_[i] == tags[j] )
          {
            return false;
          }
        }
      }


      // at this point, if we have seen must matches above, and did not meet any
      // not-must, then we are fine.  Otherwise, we need at least one match of 
      // tags_may_
      if ( tags_must_.size () != 0 )
      {
        return true;
      }
      else
      {
        for ( unsigned int i = 0; i < tags_may_.size (); i++ )
        {
          for ( unsigned int j = 0; j < tags.size (); j++ )
          {
            if ( tags_may_[i] == tags[j] )
            {
              return true;
            }
          }
        }
      }

      // nothing matched
      return false;
    }


    void log (saga::util::logging::severity  s,   // severity level of log
              std::string                    t,   // tags for log
              std::string                    m)
    {
      saga::util::the_logger::get_singleton ()->log (s, t, m);
    }

    std::ostream & logstr (saga::util::logging::severity  s,   // severity level of log
                           std::string                    t)   // tags for log
    {
      return saga::util::the_logger::get_singleton ()->logstr (s, t);
    }

  } // namespace util

} // namespace saga


