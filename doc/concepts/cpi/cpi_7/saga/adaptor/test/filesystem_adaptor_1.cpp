
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
        saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

        LOGSTR (INFO, "file_adaptor_1 ctor") << "file adaptor 1 : constructor (" << url << ")" << std::endl;

        saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
        idata->url = url;

        cc->set_state (saga::impl::call_context::Done);

        return;
      } 

      void file_adaptor_1::get_size (saga::util::shared_ptr <saga::impl::call_context> cc)
      {
        SAGA_UTIL_STACKTRACE ();
        saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

        LOGSTR (INFO, "file_adaptor_1 get_size") << "file adaptor 1 : get_size ()" << std::endl;
        saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
        LOGSTR (INFO, "file_adaptor_1 get_size") << "1" << std::endl;

        struct stat buf;
        (void) stat (idata->url.c_str (), &buf);
        LOGSTR (INFO, "file_adaptor_1 get_size") << "2" << std::endl;

        cc->set_result <int> (buf.st_size);
        LOGSTR (INFO, "file_adaptor_1 get_size") << "3" << std::endl;
        LOGSTR (INFO, "file_adaptor_1 get_size") << "3" << std::endl;
      }

      void file_adaptor_1::get_size (saga::util::shared_ptr <saga::impl::call_context> cc, 
                                     saga::async::mode                                 m)
      { 
        SAGA_UTIL_STACKTRACE ();
        LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== file adaptor 1 : get_size <> ()" << std::endl;

        cc->dump ();

        saga::util::shared_ptr <api_t>   impl (cc->get_impl ()); 
        saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();


        if ( m == saga::async::Sync )
        {
          LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== file adaptor 1 : get_size <Sync> ()" << std::endl;

          // the sync call we want to wrap is represented by just the same call
          // context as was created in the impl layer, for the default 'int get_size()'
          // We re-create it here, and pass it on the the get_size invocation
          typedef int                                           t_res_t;
          typedef saga::impl::filesystem::file                  t_api_t;
          typedef saga::impl::filesystem::file_cpi              t_cpi_t;
          typedef saga::impl::functor_0 <t_api_t, t_cpi_t, int> t_func_t;

          // create a functor which hold the cpi class' get_size() function
          // pointer.
          saga::util::shared_ptr <t_func_t> t_func (new t_func_t ("get_size", &t_cpi_t::get_size));

          // create a call context wich holds functor and implementation
          saga::util::shared_ptr <saga::impl::call_context> t_cc (new saga::impl::call_context (t_func, impl)); 
          t_cc->init_result <t_res_t> ();

          LOGSTR (DEBUG, "t_cc dump") << " ---------------------------" << std::endl; 
          t_cc->dump ();
          LOGSTR (DEBUG, "t_cc dump") << " ---------------------------" << std::endl; 
        
          saga::util::shared_ptr <saga::impl::async::task> ret (new saga::impl::async::task (t_cc));
          cc->set_result <saga::util::shared_ptr <saga::impl::async::task> > (ret);


          // call the normal sync call, 
          // this is setting result and state
          try
          {
            ret->get_instance_data ()->state = saga::async::Running;
            file_adaptor_1::get_size (t_cc); 
            ret->get_instance_data ()->state = saga::async::Done;
          }
          catch ( ... )
          {
            ret->get_instance_data ()->state = saga::async::Failed;
          }

          LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== get_size <Sync> () done ===== " << std::endl;
          return;
        }
        else if ( m == saga::async::Async ||
                  m == saga::async::Task  )
        {
          LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== file adaptor 1 : get_size <Async> ()" << std::endl;

          // async version: create a task straight away, and let the task 
          // adaptor deal with the async invocation of the sync call

          saga::util::shared_ptr <saga::impl::async::task> ret (new saga::impl::async::task (cc));
          cc->set_result <saga::util::shared_ptr <saga::impl::async::task> > (ret);

          ret->run ();

          LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== get_size <Async> () done ===== " << std::endl;

          return;
        }

        throw "incorrect call mode!? - that should never happen!";
      } 

      void file_adaptor_1::copy (saga::util::shared_ptr <saga::impl::call_context> cc,
                                 std::string                                       tgt)
      {
        SAGA_UTIL_STACKTRACE ();
        saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

        LOGSTR (INFO, "file_adaptor_1 copy") << "file adaptor 1 : copy " << tgt << std::endl;

        saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

        int res = ::system ((std::string ("cp ") + idata->url + " " + tgt).c_str ());

        if ( res != 0 )
        {
          throw "system command error"; // TODO
        }
        else
        {
          // cc->set_state (saga::impl::call_context::Done);
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
        saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

        LOGSTR (INFO, "dir_adaptor_1 ctor") << "dir adaptor 1 : constructor (" << url << ")" << std::endl;

        saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
        idata->url = url;

        cc->set_state (saga::impl::call_context::Done);

        return;
      } 

      void dir_adaptor_1::get_url (saga::util::shared_ptr <saga::impl::call_context> cc)
      {
        SAGA_UTIL_STACKTRACE ();
        saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

        saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

        LOGSTR (INFO, "dir_adaptor_1 get_url") << "dir adaptor 1 : get_url: " << idata->url << std::endl;

        cc->set_result <std::string> (idata->url);
        cc->set_state (saga::impl::call_context::Done);

        return;
      }


      void dir_adaptor_1::open (saga::util::shared_ptr <saga::impl::call_context> cc, 
                                std::string                                       url)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::util::shared_ptr <saga::impl::filesystem::file> res_t;

        saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

        saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

        std::string new_url = idata->url + "/" + url;

        LOGSTR (INFO, "dir_adaptor_1 open") << "dir adaptor 1 : open: " << new_url << std::endl;

        // FIXME: error checks
        res_t ret (new saga::impl::filesystem::file);

        // FIXME: error checks
        // FIXME: should constructor be called by the impl::dir::open, or
        // here in the adaptor?  In the impl it can go to any adaptor, from
        // here as well...
        ret->constructor (new_url);

        cc->set_result <res_t> (ret);
        cc->set_state (saga::impl::call_context::Done);

        return;
      }

    } // namespace test

  } // namespace adaptor

} // namespace saga

