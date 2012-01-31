
#include <saga/impl/async/task.hpp>
#include <saga/cpi/async/task.hpp>

#include <saga/engine/engine.hpp>

namespace saga
{
  namespace impl
  {
    namespace async
    {
      task::task (void)
      {
        SAGA_UTIL_STACKTRACE ();

        // FIXME: this c'tor should actually never be used, as it leaves t_cc_
        // unset and the task thus uninitialized.  So, remove it!
        // FIXME: OTOH, we would like to be able to have non-initialized tasks
        // on API level?

        SAGA_UTIL_STACKDUMP ();
        throw "invalid impl::task construction - need call_context parameter";
      }

      task::task (saga::util::shared_ptr <saga::impl::call_context> t_cc,
                  saga::util::shared_ptr <saga::impl::engine>       t_engine_)
      {
        SAGA_UTIL_STACKTRACE ();

        t_cc_     = t_cc;
        t_engine_ = t_engine_;
      }

      task::~task (void)
      {
      }

      void_t task::constructor (void)
      {
        SAGA_UTIL_STACKTRACE ();
      }


      saga::async::state task::get_state (void)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::async::state                          res_t;
        typedef saga::impl::async::task                     api_t;
        typedef saga::impl::async::task_cpi                 cpi_t;
        typedef saga::impl::func_0 <api_t, cpi_t, res_t> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("get_state", &cpi_t::get_state));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (shared_this <api_t> (), func)); 

        engine_->call /* <cpi_t> */ (cc);

        if ( cc->get_state () == saga::async::Failed )
        {
          SAGA_UTIL_STACKDUMP ();
          throw " task::get_state failed - can't get state";
        }

        return cc->get_func ()->get_result <res_t> ();
      }

      saga::util::shared_ptr <result_t> task::get_result (void)
      {
        SAGA_UTIL_STACKTRACE ();

        // FIXME: t_cc_->wait ();
        t_cc_->dump ();

        return (t_cc_->get_func ()->get_result ());

        // typedef saga::util::shared_ptr <result_t>           res_t;
        // typedef saga::impl::async::task                     api_t;
        // typedef saga::impl::async::task_cpi                 cpi_t;
        // typedef saga::impl::func_0 <api_t, cpi_t, res_t> func_t;

        // saga::util::shared_ptr <func_t> func (new func_t ("get_result", &cpi_t::get_result));

        // saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (shared_this <api_t> (), func)); 

        // engine_->call /* <cpi_t> */ (cc);

        // if ( cc->get_state () == saga::async::Failed )
        // {
        //   SAGA_UTIL_STACKDUMP ();
        //   throw " task::get_state indicates failed";
        // }

        // return cc->get_func ()->get_result <res_t> ();
      }


      void_t task::run (void)
      {
        typedef saga::impl::void_t                          res_t;
        typedef saga::impl::async::task                     api_t;
        typedef saga::impl::async::task_cpi                 cpi_t;
        typedef saga::impl::func_0 <api_t, cpi_t, res_t> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("run", &cpi_t::run));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (shared_this <api_t> (), func)); 

        engine_->call /* <cpi_t> */ (cc);

        if ( cc->get_state () == saga::async::Failed )
        {
          SAGA_UTIL_STACKDUMP ();
          throw " task::run () failed";
        }
      }

    } // namespace async

  } // namespace impl

} // namespace saga

