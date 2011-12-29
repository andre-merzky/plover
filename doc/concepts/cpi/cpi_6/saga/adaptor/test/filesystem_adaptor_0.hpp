
#ifndef SAGA_ADAPTOR_FILESYSTEM_0_HPP
#define SAGA_ADAPTOR_FILESYSTEM_0_HPP

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
      class file_adaptor_0 : public saga::impl::filesystem::file_cpi
      {
        private:
          typedef saga::impl::void_t                         void_t;
          typedef saga::impl::filesystem::file               api_t;
          typedef saga::impl::filesystem::file_cpi           cpi_t;
          typedef saga::impl::filesystem::file_instance_data idata_t;

        public:
          file_adaptor_0    (void) { } 
          ~file_adaptor_0   (void) { } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc, 
                              std::string                           url) 
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "file_adaptor_0 ctor") << "file adaptor 0 : constructor (" << url << ")" << std::endl;
            
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            cc->set_call_state (saga::impl::Done);

            return;
          } 

          void get_size (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            cc->set_call_state (saga::impl::Failed);

            LOGSTR (INFO, "file_adaptor_0 get_size") << "file adaptor 0 : get_size" << std::endl;
            throw "oops";
          }

          void get_size (saga::util::shared_ptr <saga::impl::call_context> cc, 
                         saga::impl::call_mode                             m)
          { 
            SAGA_UTIL_STACKTRACE ();

            cc->set_call_state (saga::impl::Failed);

            LOGSTR (INFO, "file_adaptor_0 get_size async") << "file adaptor 0 : get_size <async>" << std::endl;
            throw "oops";

          } 

          void copy (saga::util::shared_ptr <saga::impl::call_context> cc,
                     std::string                                       tgt)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            cc->set_call_state (saga::impl::Failed);

            LOGSTR (INFO, "file_adaptor_0 copy") << "file adaptor 0 : copy " << tgt << std::endl;
            throw "oops";
            return;
          }
      };



      class dir_adaptor_0 : public saga::impl::filesystem::dir_cpi
      {
        private:
          typedef saga::impl::void_t                        void_t;
          typedef saga::impl::filesystem::dir               api_t;
          typedef saga::impl::filesystem::dir_cpi           cpi_t;
          typedef saga::impl::filesystem::dir_instance_data idata_t;

        public:
          dir_adaptor_0    (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 
          
          ~dir_adaptor_0   (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                            std::string                                       url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "dir_adaptor_0 ctor") << "dir adaptor 0 : constructor (" << url << ")" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            cc->set_call_state (saga::impl::Done);

            return;
          } 

          void get_url (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

            LOGSTR (INFO, "dir_adaptor_0 get_url") << "dir adaptor 0 : get_url" << std::endl;
            throw "oops";
          }
      };

    } // namespace test

  } // namespace adaptor

} // namespace saga

#endif //  SAGA_ADAPTOR_FILESYSTEM_0_HPP

