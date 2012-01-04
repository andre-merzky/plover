
#include <saga/cpi/async/task.hpp>

#include "task.hpp"

namespace saga
{
  namespace impl
  {
    namespace async
    {
      task::task (void)
      {
        SAGA_UTIL_STACKTRACE ();
      }

      task::task (saga::util::shared_ptr <saga::impl::call_context> t_cc_)
        : idata_  (new task_instance_data)
      {
        SAGA_UTIL_STACKTRACE ();

        saga::util::scoped_lock sl (idata_->get_mutex ());

        idata_->t_cc = t_cc_;
      }


      void_t task::constructor (void)
      {
        SAGA_UTIL_STACKTRACE ();
      }


      saga::async::state task::get_state (void)
      {
        SAGA_UTIL_STACKTRACE ();

        saga::util::scoped_lock sl (idata_->get_mutex ());

        typedef saga::async::state                      res_t;
        typedef saga::impl::async::task                     api_t;
        typedef saga::impl::async::task_cpi                 cpi_t;
        typedef saga::impl::functor_0 <api_t, cpi_t, res_t> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("get_state", &cpi_t::get_state));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          throw " task::get_state failed - can't get state";
        }

        return cc->get_result <res_t> ();
      }

      saga::util::shared_ptr <result_t> task::get_result (void)
      {
        SAGA_UTIL_STACKTRACE ();

        saga::util::scoped_lock sl (idata_->get_mutex ());

        typedef saga::util::shared_ptr <result_t>            res_t;
        typedef saga::impl::async::task                      api_t;
        typedef saga::impl::async::task_cpi                  cpi_t;
        typedef saga::impl::functor_0 <api_t, cpi_t, res_t > func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("get_result", &cpi_t::get_result));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          throw " task::get_state indicates failed";
        }

        return cc->get_result <res_t> ();
      }

      void_t task::run (void)
      {
        saga::util::scoped_lock sl (idata_->get_mutex ());

        typedef saga::impl::void_t                           res_t;
        typedef saga::impl::async::task                      api_t;
        typedef saga::impl::async::task_cpi                  cpi_t;
        typedef saga::impl::functor_0 <api_t, cpi_t, void_t> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("run", &cpi_t::run));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_state () == saga::impl::call_context::Failed )
        {
          throw " task::run () failed";
        }
      }

    } // namespace async

  } // namespace impl

} // namespace saga

