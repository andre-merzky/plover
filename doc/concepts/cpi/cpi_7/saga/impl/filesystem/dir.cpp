
#include <saga/cpi/filesystem/dir.hpp>

#include "dir.hpp"
  
namespace saga
{
  namespace impl
  {
    namespace filesystem
    {
      //////////////////////////////////////////////////////////////////
      dir::dir (void) // FIXME: the dir impl and pimpl object hierarchies are ignored
        : idata_ (new dir_instance_data)
      {
        SAGA_UTIL_STACKTRACE ();

        idata_->valid = false;
        idata_->url   = "";
      }

      //////////////////////////////////////////////////////////////////
      void_t dir::constructor (std::string url)
      {
        SAGA_UTIL_STACKTRACE ();

        // lock scope
        {
          saga::util::scoped_lock sl (idata_->get_mutex ());
          idata_->url   = url;
          idata_->valid = true;
        } 
        // lock scope

        typedef saga::impl::void_t                                       res_t;
        typedef saga::impl::filesystem::dir                              api_t;
        typedef saga::impl::filesystem::dir_cpi                          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, res_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("constructor",&cpi_t::constructor, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (shared_this <api_t> ())); 

        cc->set_mode   (saga::async::Sync);              // this is a sync call
        cc->set_state  (saga::impl::call_context::New);  // we just created the cc
        cc->set_policy (saga::impl::call_context::Any);  // any successfull adaptor can do the job

        engine_->call <api_t, cpi_t> (func, cc);

        // check if the call was completed
        while ( cc->get_state () == saga::impl::call_context::Running )
        {
          // this is a sync call, so we just wait it out
          ::sleep (1);
        }
        
        // check if the call was completed all right
        if ( cc->get_state () == saga::impl::call_context::Failed )
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
        typedef saga::impl::functor_0 <api_t, cpi_t, res_t> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("get_url", &cpi_t::get_url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (shared_this <api_t> ())); 

        engine_->call <api_t, cpi_t> (func, cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          SAGA_UTIL_STACKDUMP ();
          throw " dir::get_url () indicates failed";
        }

        return cc->get_result <res_t> ();
      }

      //////////////////////////////////////////////////////////////////
      saga::util::shared_ptr <saga::impl::filesystem::file> dir::open (std::string url)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::util::shared_ptr <saga::impl::filesystem::file>    res_t;
        typedef saga::impl::filesystem::dir                              api_t;
        typedef saga::impl::filesystem::dir_cpi                          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, res_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("open", &cpi_t::open, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (shared_this <api_t> ())); 

        engine_->call <api_t, cpi_t> (func, cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          SAGA_UTIL_STACKDUMP ();
          throw " dir::open () indicates failed";
        }

        return cc->get_result <res_t> ();
      }

    } // namespace filesystem

  } // namespace impl

} // namespace saga

