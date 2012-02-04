
#include <saga/util/stack_tracer.hpp>

#define MAX_INDENT 15

namespace saga
{
  namespace util
  {
    /// // FIXME: we should maintain one stacktrace per thread, obviously
    /// static std::vector <std::string> & get_stack_ (void)
    /// {
    ///   static std::vector <std::string> stack_;
    ///   return stack_;
    /// }

    unsigned int stack_tracer::indent_ = 0;
    bool         stack_tracer::enabled = false;

    std::string stack_tracer::indent (void)
    {
      std::string ret;

      if ( enabled ) { for ( unsigned int i = 0; i < indent_; i++ ) { ret +=
        " "; } }

      return ret;
    }

    std::string stack_tracer::unindent (void)
    {
      std::string ret;

      if ( enabled )
      {
        for ( unsigned int i = indent_; i < MAX_INDENT; i++ )
        {
          ret += " ";
        }
      }

      return ret;
    }


    stack_tracer::stack_tracer (const char * sig, 
                                const char * file, 
                                int          line, 
                                const char * msg)
      : sig_      (sig)
        , file_     (file)
        , line_     (line)
        , msg_      (msg)
    {
      std::string out;

      indent_++;

      if ( enabled )
      {
        out += indent ();
        out += " -> ";
        out += unindent ();

        if ( 0 == sizeof (msg_) )
        {
          out += msg_;
          out += "\n";
          out += indent ();
          out += "  > \n";
          out += unindent ();
        }

        out += sig_;
        out += " : ";
        out += file_;
        out += " +";
        out += saga::util::itoa (line_);

        saga::util::log (saga::util::logging::Debug, "stack trace level" + saga::util::itoa (indent_), out);

        /// get_stack_ ().push_back (sig_ + " - " + msg_);
      }
    }

    stack_tracer::~stack_tracer (void)
    {
      if ( enabled )
      {
        std::string out;

        out += indent ();
        out += " <- ";
        out += unindent ();
        out += sig_;
        out += " : ";
        out += file_;
        out += " +";
        out += saga::util::itoa (line_);

        saga::util::log (saga::util::logging::Debug, "stack trace level" + saga::util::itoa (indent_), out);

        /// get_stack_ ().pop_back ();
      }

      indent_--;
    }

    void stack_tracer::dump (void)
    {
      saga::util::logstr (DEBUG, "stack trace print") 
        << "---------------------------------------" << std::endl;

      /// for ( unsigned int i = 0; i < get_stack_ ().size (); i++ )
      /// {
      ///   saga::util::logstr (DEBUG, "stack trace print") 
      ///     << i << "\t" << get_stack_ ()[i] << std::endl;
      /// }

      saga::util::logstr (DEBUG, "stack trace print") 
        << "---------------------------------------" << std::endl;
    }

  } // namespace util

} // namespace saga

