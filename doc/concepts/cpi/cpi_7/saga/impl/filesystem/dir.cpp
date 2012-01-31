
#include <saga/cpi/filesystem/dir.hpp>
#include <saga/impl/filesystem/dir.hpp>
  
namespace saga
{
  namespace impl
  {
    namespace filesystem
    {
      //////////////////////////////////////////////////////////////////
      dir::dir (void) // FIXME: the dir impl and pimpl object hierarchies are ignored
      {
        SAGA_UTIL_STACKTRACE ();

        valid_ = false;
        url_   = "";
      }

      //////////////////////////////////////////////////////////////////
      void_t dir::constructor (std::string url)
      {
        SAGA_UTIL_STACKTRACE ();

        // lock scope
        {
          url_   = url;
          valid_ = true;
        } 
        // lock scope

        typedef saga::impl::void_t                                       res_t;
        typedef saga::impl::filesystem::dir                              api_t;
        typedef saga::impl::filesystem::dir_cpi                          cpi_t;
        typedef saga::impl::func_1 <api_t, cpi_t, res_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("constructor",&cpi_t::constructor, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (shared_this <api_t> (), func)); 

        cc->set_mode   (saga::async::Sync); // this is a sync call
        cc->set_state  (saga::async::New);  // we just created the cc
        cc->set_policy (saga::impl::call_context::Any);  // any successfull adaptor can do the job

        engine_->call /* <cpi_t> */ (cc);

        // check if the call was completed
        while ( cc->get_state () == saga::async::Running )
        {
          // this is a sync call, so we just wait it out
          ::sleep (1);
        }
        
        // check if the call was completed all right
        if ( cc->get_state () == saga::async::Failed )
        {
          SAGA_UTIL_STACKDUMP ();
          throw " dir::constructor () indicates failed";
        }

        // here we are Done, and have a result
      }

      //////////////////////////////////////////////////////////////////
      std::string dir::get_url (void)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef std::string                                 res_t;
        typedef saga::impl::filesystem::dir                 api_t;
        typedef saga::impl::filesystem::dir_cpi             cpi_t;
        typedef saga::impl::func_0 <api_t, cpi_t, res_t> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("get_url", &cpi_t::get_url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (shared_this <api_t> (), func)); 

        engine_->call /* <cpi_t> */ (cc);

        if ( cc->get_state () == saga::async::Failed )
        {
          SAGA_UTIL_STACKDUMP ();
          throw " dir::get_url () indicates failed";
        }

        return cc->get_func ()->get_result <res_t> ();
      }

      //////////////////////////////////////////////////////////////////
      saga::util::shared_ptr <saga::impl::filesystem::file> dir::open (std::string url)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::util::shared_ptr <saga::impl::filesystem::file>    res_t;
        typedef saga::impl::filesystem::dir                              api_t;
        typedef saga::impl::filesystem::dir_cpi                          cpi_t;
        typedef saga::impl::func_1 <api_t, cpi_t, res_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("open", &cpi_t::open, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (shared_this <api_t> (), func)); 

        engine_->call /* <cpi_t> */ (cc);

        if ( cc->get_state () == saga::async::Failed )
        {
          SAGA_UTIL_STACKDUMP ();
          throw " dir::open () indicates failed";
        }

        return cc->get_func ()->get_result <res_t> ();
      }

    } // namespace filesystem

  } // namespace impl

} // namespace saga

