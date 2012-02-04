
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "filesystem_adaptor_1.hpp"

namespace saga
{
  namespace adaptor
  {
    namespace test // test adaptprs
    {
      file_adaptor_1::file_adaptor_1    (void) 
      {
        SAGA_UTIL_STACKTRACE ();
      } 

      file_adaptor_1::~file_adaptor_1   (void) 
      { 
        SAGA_UTIL_STACKTRACE ();
      } 

      void file_adaptor_1::constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                                        std::string                                       url) 
      { 
        SAGA_UTIL_STACKTRACE ();

        LOGSTR (INFO, "file_adaptor_1 ctor") << "file adaptor 1 : constructor (" << url << ")" << std::endl;

        saga::util::shared_ptr <api_t>   impl  = cc->get_impl (); 

        impl->url_ = url;

        cc->set_state (saga::async::Done);

        return;
      } 


      //
      // in this model, we force all async adaptor calls to implement 
      // The BIG Switch.  That is not terrible, but one could also consider to
      // overload three different function calls, like
      //
      //   void get_size (saga::impl::sync_tag_t  tag , saga::impl::call_context cc);
      //   void get_size (saga::impl::async_tag_t tag , saga::impl::call_context cc);
      //   void get_size (saga::impl::task_tag_t  tag , saga::impl::call_context cc);
      //
      // and do the switch on impl level.  
      //
      void file_adaptor_1::get_size (saga::util::shared_ptr <saga::impl::call_context> cc)
      { 
        SAGA_UTIL_STACKTRACE ();

        switch ( cc->get_mode () )
        {
          case saga::async::Async :
          case saga::async::Task :
            {
              LOGSTR (INFO, "file_adaptor_1 get_size") << "===== file adaptor 1 : get_size <Async> ()" << std::endl;

              // async version: we let the task adaptor deal with the async invocation of the sync call
              throw "NotImplemented";
              break;
            }

          case saga::async::Sync :
            {
              LOGSTR (INFO, "file_adaptor_1 get_size") << "file adaptor 1 : get_size ()" << std::endl;

              saga::util::shared_ptr <api_t> impl  = cc->get_impl (); 

              struct stat buf;
              (void) ::stat (impl->url_.c_str (), &buf);

              {
                // FIXME: scoped lock for cc.  always.
                cc->get_func ()->set_result <size_t> (buf.st_size);
                cc->set_state        (saga::async::Done);
              }

              LOGSTR (INFO, "file_adaptor_1 get_size") << "===== get_size <Sync> () done ===== " << std::endl;
              break;
            }
        }
      } 

      void file_adaptor_1::copy (saga::util::shared_ptr <saga::impl::call_context> cc,
                                 std::string                                       tgt)
      {
        SAGA_UTIL_STACKTRACE ();

        LOGSTR (INFO, "file_adaptor_1 copy") << "file adaptor 1 : copy " << tgt << std::endl;

        saga::util::shared_ptr <api_t>   impl  = cc->get_impl (); 

        int res = ::system ((std::string ("cp ") + impl->url_ + " " + tgt).c_str ());

        if ( res != 0 )
        {
          SAGA_UTIL_STACKDUMP ();
          throw "system command error"; // TODO
        }
        else
        {
          // cc->set_state (saga::async::Done);
        }

        return;
      }


      dir_adaptor_1::dir_adaptor_1 (void) 
      { 
        SAGA_UTIL_STACKTRACE ();
      } 

      dir_adaptor_1::~dir_adaptor_1 (void) 
      { 
        SAGA_UTIL_STACKTRACE ();
      } 

      void dir_adaptor_1::constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                                       std::string                                       url) 
      { 
        SAGA_UTIL_STACKTRACE ();

        LOGSTR (INFO, "dir_adaptor_1 ctor") << "dir adaptor 1 : constructor (" << url << ")" << std::endl;

        saga::util::shared_ptr <api_t>   impl  = cc->get_impl (); 

        impl->url_ = url;

        cc->set_state (saga::async::Done);

        return;
      } 

      void dir_adaptor_1::get_url (saga::util::shared_ptr <saga::impl::call_context> cc)
      {
        SAGA_UTIL_STACKTRACE ();
        saga::util::shared_ptr <api_t>   impl  = cc->get_impl ();  

        LOGSTR (INFO, "dir_adaptor_1 get_url") << "dir adaptor 1 : get_url: " << impl->url_ << std::endl;

        cc->get_func ()->set_result <std::string> (impl->url_);
        cc->set_state                (saga::async::Done);

        return;
      }


      void dir_adaptor_1::open (saga::util::shared_ptr <saga::impl::call_context> cc, 
                                std::string                                       url_)
      {
        SAGA_UTIL_STACKTRACE ();


        typedef saga::util::shared_ptr <saga::impl::filesystem::file> res_t;

        saga::util::shared_ptr <api_t>   impl  = cc->get_impl ();  

        std::string new_url = impl->url_ + "/" + url_;

        LOGSTR (INFO, "dir_adaptor_1 open") << "1--------------------------------------------" << std::endl;
        LOGSTR (INFO, "dir_adaptor_1 open") << "dir adaptor 1 : open: " << new_url << std::endl;

        // FIXME: error checks
        res_t ret (new saga::impl::filesystem::file);

        LOGSTR (INFO, "dir_adaptor_1 open") << "2--------------------------------------------" << std::endl;

        // FIXME: error checks
        // FIXME: should constructor be called by the impl::dir::open, or
        // here in the adaptor?  In the impl it can go to any adaptor, from
        // here as well...
        ret->constructor (new_url);

        LOGSTR (INFO, "dir_adaptor_1 open") << "3--------------------------------------------" << std::endl;
        cc.dump ();
        cc->dump ();
        LOGSTR (INFO, "dir_adaptor_1 open") << "4--------------------------------------------" << std::endl;

        cc->get_func ()->set_result <res_t> (ret);
        cc->set_state (saga::async::Done);

        LOGSTR (INFO, "dir_adaptor_1 open") << "5--------------------------------------------" << std::endl;

        return;
      }

    } // namespace test

  } // namespace adaptor

} // namespace saga

