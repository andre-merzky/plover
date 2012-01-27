
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

        typedef saga::impl::void_t                                       res_t;
        typedef saga::impl::filesystem::file                             api_t;
        typedef saga::impl::filesystem::file_cpi                         cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, res_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("constructor", &cpi_t::constructor, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (shared_this <api_t> ())); 

        engine_->call <api_t, cpi_t> (func, cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          SAGA_UTIL_STACKDUMP ();
          throw " file::constructor indicates failed";
        }

      }

      //////////////////////////////////////////////////////////////////
      saga::util::shared_ptr <saga::impl::async::task> file::get_size (saga::async::mode m)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::util::shared_ptr <saga::impl::async::task>  res_t;
        typedef saga::impl::filesystem::file                      api_t;
        typedef saga::impl::filesystem::file_cpi                  cpi_t;
        typedef saga::impl::functor_0 <api_t, cpi_t, res_t>       func_t;

        // create a functor which hold the cpi class' get_size() function
        // pointer.  The second templ parameter is the functions return type
        saga::util::shared_ptr <func_t> func (new func_t ("get_size", &cpi_t::get_size));

        // create a call context wich holds the impl
        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (shared_this <api_t> ())); 

        // TODO: those should go into the cc c'tor
        cc->set_mode   (m);
        cc->set_policy (saga::impl::call_context::Any);  // any successfull adaptor can do the job

        // func and cc are given to the engine, so it can use the functor to call that
        // function on some cpi
        engine_->call <api_t, cpi_t> (func, cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          SAGA_UTIL_STACKDUMP ();
          throw " file::get_size <> () indicates failed";
        }

        res_t ret (new saga::impl::async::task (cc));

        // ensure that the returned task's state matches the mode m.
        switch ( m )
        {
          case saga::async::Sync  : ret->run  (); /* FIXME: ret->wait ();*/ break; 
          case saga::async::Async : ret->run  ();               break; 
          case saga::async::Task  :                             break; 
        }

        // TODO: if a async/task call returns with NotImplemented, then try to
        // invoke a task.run adaptor with the Sync version no the cc as arg

        return ret;
      }

      //////////////////////////////////////////////////////////////////
      void_t file::copy (std::string tgt)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::impl::void_t                                       res_t;
        typedef saga::impl::filesystem::file                             api_t;
        typedef saga::impl::filesystem::file_cpi                         cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, res_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("copy", &cpi_t::copy, tgt));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (shared_this <api_t> ())); 

        engine_->call <api_t, cpi_t> (func, cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          SAGA_UTIL_STACKDUMP ();
          throw " file::copy () indicates failed";
        }

        // FIXME: nuclear when void_t result types get allocated
        // return cc->get_result <res_t> ();
      }

    } // namespace filesystem

  } // namespace impl

} // namespace saga

