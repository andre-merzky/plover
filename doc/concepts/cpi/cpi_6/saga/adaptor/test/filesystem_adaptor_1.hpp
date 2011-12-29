
#ifndef SAGA_ADAPTOR_FILESYSTEM_1_HPP
#define SAGA_ADAPTOR_FILESYSTEM_1_HPP

#include <map>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <saga/util/mutex.hpp>
#include <saga/util/scoped_lock.hpp>
#include <saga/util/shareable.hpp>
#include <saga/util/enums.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

namespace saga
{
  namespace adaptor
  {
    namespace test // test adaptprs
    {
      class file_adaptor_1 : public saga::impl::filesystem::file_cpi
      {
        private:
          typedef saga::impl::void_t                         void_t;
          typedef saga::impl::filesystem::file               api_t;
          typedef saga::impl::filesystem::file_cpi           cpi_t;
          typedef saga::impl::filesystem::file_instance_data idata_t;

        public:
          file_adaptor_1    (void) { } 
          ~file_adaptor_1   (void) { } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                            std::string                                       url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "file_adaptor_1 ctor") << "file adaptor 1 : constructor (" << url << ")" << std::endl;
            
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            cc->set_call_state (saga::impl::Done);

            return;
          } 

          void get_size (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "file_adaptor_1 get_size") << "file adaptor 1 : get_size ()" << std::endl;
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            struct stat buf;
            (void) stat (idata->url.c_str (), &buf);

            cc->set_result <int> (buf.st_size);
            cc->set_call_state (saga::impl::Done);
          }

          void get_size (saga::util::shared_ptr <saga::impl::call_context> cc, 
                         saga::impl::call_mode                             m)
          { 
            SAGA_UTIL_STACKTRACE ();
            LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== file adaptor 1 : get_size <> ()" << std::endl;

            cc->set_mode (m);

            cc->dump ();

            if ( m == saga::impl::Sync )
            {
              LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== file adaptor 1 : get_size <Sync> ()" << std::endl;

              // call the normal sync call, 
              // this is setting result and state
              get_size (cc); 
              cc->set_call_state (saga::impl::Done);

              saga::util::shared_ptr <saga::impl::task> ret (new saga::impl::task (cc));

              cc->set_result <saga::util::shared_ptr <saga::impl::task> > (ret);

              LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== get_size <Sync> () done ===== " << std::endl;
              return;
            }
            else if ( m == saga::impl::Async ||
                      m == saga::impl::Task  )
            {
              LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== file adaptor 1 : get_size <Async> ()" << std::endl;

              // async version: create a task straight away, and let the task 
              // adaptor deal with the async invocation of the sync call
              
              saga::util::shared_ptr <saga::impl::task> ret (new saga::impl::task (cc));

              cc->set_call_state (saga::impl::Done);

              cc->set_result <saga::util::shared_ptr <saga::impl::task> > (ret);

              LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== get_size <Async> () done ===== " << std::endl;

              return;
            }

            throw "incorrect call mode!? - that should never happen!";
          } 

          void copy (saga::util::shared_ptr <saga::impl::call_context> cc,
                     std::string                                       tgt)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "file_adaptor_1 copy") << "file adaptor 1 : copy " << tgt << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            int res = ::system ((std::string ("cp ") + idata->url + " " + tgt).c_str ());

            if ( res != 0 )
            {
              cc->set_call_state (saga::impl::Failed);
              // cc->set_error ("system command error"); // TODO
            }
            else
            {
              cc->set_call_state (saga::impl::Done);
            }

            return;
          }
      };

      class dir_adaptor_1 : public saga::impl::filesystem::dir_cpi
      {
        private:
          typedef saga::impl::void_t                        void_t;
          typedef saga::impl::filesystem::dir               api_t;
          typedef saga::impl::filesystem::dir_cpi           cpi_t;
          typedef saga::impl::filesystem::dir_instance_data idata_t;

        public:
          dir_adaptor_1    (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 

          ~dir_adaptor_1   (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                            std::string                                       url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "dir_adaptor_1 ctor") << "dir adaptor 1 : constructor (" << url << ")" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            cc->set_call_state (saga::impl::Done);

            return;
          } 

          void get_url (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            LOGSTR (INFO, "dir_adaptor_1 get_url") << "dir adaptor 1 : get_url: " << idata->url << std::endl;

            cc->set_result <std::string> (idata->url);
            cc->set_call_state (saga::impl::Done);

            return;
          }


          void open (saga::util::shared_ptr <saga::impl::call_context> cc, 
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
            cc->set_call_state (saga::impl::Done);

            return;
          }
      };

    } // namespace test

  } // namespace adaptor

} // namespace saga

#endif //  SAGA_ADAPTOR_FILESYSTEM_1_HPP

