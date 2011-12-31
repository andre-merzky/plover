
#include "filesystem_adaptor_0.hpp"

namespace saga
{
  namespace adaptor
  {
    namespace test // test adaptprs
    {
      void file_adaptor_0::constructor (saga::util::shared_ptr <saga::impl::call_context> cc, 
                                        std::string                                       url)
      {
        SAGA_UTIL_STACKTRACE ();
        saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

        LOGSTR (INFO, "file_adaptor_0 ctor") << "file adaptor 0 : constructor (" << url << ")" << std::endl;

        saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
        idata->url = url;

        cc->set_call_state (saga::async::Done);

        return;
      } 

      void file_adaptor_0::get_size    (saga::util::shared_ptr <saga::impl::call_context> cc)
      {
        SAGA_UTIL_STACKTRACE ();
        saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

        cc->set_call_state (saga::async::Failed);

        LOGSTR (INFO, "file_adaptor_0 get_size") << "file adaptor 0 : get_size" << std::endl;
        throw "oops";
      }

      void file_adaptor_0::get_size (saga::util::shared_ptr <saga::impl::call_context> cc, 
                                     saga::async::mode                                  m)
      { 
        SAGA_UTIL_STACKTRACE ();

        cc->set_call_state (saga::async::Failed);

        LOGSTR (INFO, "file_adaptor_0 get_size async") << "file adaptor 0 : get_size <async>" << std::endl;
        throw "oops";

      } 

      void file_adaptor_0::copy (saga::util::shared_ptr <saga::impl::call_context> cc,
                                 std::string                                       tgt)
      {
        SAGA_UTIL_STACKTRACE ();
        saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

        cc->set_call_state (saga::async::Failed);

        LOGSTR (INFO, "file_adaptor_0 copy") << "file adaptor 0 : copy " << tgt << std::endl;
        throw "oops";
        return;
      }

      dir_adaptor_0::dir_adaptor_0    (void) 
      { 
        SAGA_UTIL_STACKTRACE ();
      } 

      dir_adaptor_0::~dir_adaptor_0   (void) 
      { 
        SAGA_UTIL_STACKTRACE ();
      } 

      void dir_adaptor_0::constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                                       std::string                                       url) 
      { 
        SAGA_UTIL_STACKTRACE ();
        saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

        LOGSTR (INFO, "dir_adaptor_0 ctor") << "dir adaptor 0 : constructor (" << url << ")" << std::endl;

        saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
        idata->url = url;

        cc->set_call_state (saga::async::Done);

        return;
      } 

      void dir_adaptor_0::get_url (saga::util::shared_ptr <saga::impl::call_context> cc)
      {
        SAGA_UTIL_STACKTRACE ();
        saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

        LOGSTR (INFO, "dir_adaptor_0 get_url") << "dir adaptor 0 : get_url" << std::endl;
        throw "oops";
      }


    } // namespace test

  } // namespace adaptor

} // namespace saga

