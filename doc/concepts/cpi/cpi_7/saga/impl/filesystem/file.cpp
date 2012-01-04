
#include <saga/cpi/filesystem/file.hpp>

#include "file.hpp"
  
namespace saga
{
  namespace impl
  {
    namespace filesystem
    {
      //////////////////////////////////////////////////////////////////
      file::file (void)                    // FIXME: the file impl and pimpl object hierarchies are ignored
        : idata_ (new file_instance_data)
      {
        SAGA_UTIL_STACKTRACE ();

        idata_->valid = false;
        idata_->url   = "";
        idata_->pos   = 0;
      }

      //////////////////////////////////////////////////////////////////
      void_t file::constructor (std::string url)
      {
        SAGA_UTIL_STACKTRACE ();

        saga::util::scoped_lock sl (idata_->get_mutex ());

        idata_->url   = url;
        idata_->pos   = 0;
        idata_->valid = true;

        typedef saga::impl::void_t                                        res_t;
        typedef saga::impl::filesystem::file                              api_t;
        typedef saga::impl::filesystem::file_cpi                          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, void_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("constructor", &cpi_t::constructor, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          throw " file::constructor indicates failed";
        }

        return cc->get_result <res_t> ();
      }

      //////////////////////////////////////////////////////////////////
      int file::get_size (void)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef int                                       res_t;
        typedef saga::impl::filesystem::file              api_t;
        typedef saga::impl::filesystem::file_cpi          cpi_t;
        typedef saga::impl::functor_0 <api_t, cpi_t, int> func_t;

        // create a functor which hold the cpi class' get_size() function
        // pointer.
        saga::util::shared_ptr <func_t> func (new func_t ("get_size", &cpi_t::get_size));

        // create a call context wich holds functor and implementation
        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->set_mode    (saga::impl::call_context::Sync);

        // FIXME: move result from cc to func, removing this templated call
        cc->init_result <res_t> ();

        LOGSTR (DEBUG, "cc dump") << " ---------------------------" << std::endl; 
        func->dump ();
        LOGSTR (DEBUG, "cc dump") << " ---------------------------" << std::endl; 
        cc->dump ();
        LOGSTR (DEBUG, "cc dump") << " ---------------------------" << std::endl; 

        // the cc is given to the engine, so it can use the functor to call that
        // function on some cpi
        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          throw " file::get_size indicates failed";
        }

        return cc->get_result <res_t> ();
      }

      //////////////////////////////////////////////////////////////////
      saga::util::shared_ptr <saga::impl::async::task> file::get_size (saga::async::mode m)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::util::shared_ptr <saga::impl::async::task>                                                          res_t;
        typedef saga::impl::filesystem::file                                                                              api_t;
        typedef saga::impl::filesystem::file_cpi                                                                          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, saga::util::shared_ptr <saga::impl::async::task>, saga::async::mode> func_t;

        // create a functor which hold the cpi class' get_size() function
        // pointer.  The second templ parameter is the functions return type
        saga::util::shared_ptr <func_t> func (new func_t ("get_size", &cpi_t::get_size, m));

        // create a call context wich holds functor and implementation
        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        // the cc is given to the engine, so it can use the functor to call that
        // function on some cpi
        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          throw " file::get_size <> () indicates failed";
        }

        res_t ret = cc->get_result <res_t> ();

        // ret.dump ();
        // cc.dump ();
        // cc->dump ();

        return ret;
      }

      //////////////////////////////////////////////////////////////////
      void_t file::copy (std::string tgt)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::impl::void_t                        res_t;
        typedef saga::impl::filesystem::file              api_t;
        typedef saga::impl::filesystem::file_cpi          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, void_t, std::string>    func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("copy", &cpi_t::copy, tgt));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          throw " file::copy () indicates failed";
        }

        return cc->get_result <res_t> ();
      }

    } // namespace filesystem

  } // namespace impl

} // namespace saga

