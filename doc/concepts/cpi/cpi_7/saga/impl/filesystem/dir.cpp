
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
        idata_->pos   = 0;
      }

      //////////////////////////////////////////////////////////////////
      void_t dir::constructor (std::string url)
      {
        SAGA_UTIL_STACKTRACE ();

        saga::util::scoped_lock sl (idata_->get_mutex ());
        idata_->url   = url;
        idata_->pos   = 0;
        idata_->valid = true;

        typedef saga::impl::void_t                          res_t;
        typedef saga::impl::filesystem::dir                 api_t;
        typedef saga::impl::filesystem::dir_cpi             cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, 
                                       void_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("constructor",&cpi_t::constructor, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);
        
        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          throw " dir::constructor () indicates failed";
        }

        return cc->get_result <res_t> ();
      }

      //////////////////////////////////////////////////////////////////
      std::string dir::get_url (void)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef std::string                                       res_t;
        typedef saga::impl::filesystem::dir                       api_t;
        typedef saga::impl::filesystem::dir_cpi                   cpi_t;
        typedef saga::impl::functor_0 <api_t, cpi_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("get_url", &cpi_t::get_url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          throw " dir::get_url () indicates failed";
        }

        return cc->get_result <res_t> ();
      }

      //////////////////////////////////////////////////////////////////
      saga::util::shared_ptr <saga::impl::filesystem::file> dir::open (std::string url)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::util::shared_ptr <saga::impl::filesystem::file> res_t;
        typedef saga::impl::filesystem::dir                           api_t;
        typedef saga::impl::filesystem::dir_cpi                       cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, 
                                       saga::util::shared_ptr <saga::impl::filesystem::file>, std::string > func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("open", &cpi_t::open, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          throw " dir::open () indicates failed";
        }

        return cc->get_result <res_t> ();
      }

    } // namespace filesystem

  } // namespace impl

} // namespace saga

