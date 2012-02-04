
#ifndef SAGA_IMPL_TASK_TASK_HPP
#define SAGA_IMPL_TASK_TASK_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/api/async/state.hpp>
#include <saga/engine/call_context.hpp>

namespace saga
{
  namespace impl
  {
    class engine;

    namespace async
    {
      // FIXME: impl_base should also provide lockable, so that we can lock
      // a mutex member for, e.g., idata settings
      class task : public saga::impl::impl_base
      {
        public:
          // instance data
          // FIXME: make private
          saga::util::shared_ptr <saga::impl::call_context> t_cc_;     // context the task operates on
          saga::util::shared_ptr <saga::impl::engine>       t_engine_; // engine  the task operates with

          task (void);
          task (saga::util::shared_ptr <saga::impl::call_context> t_cc_,
                saga::util::shared_ptr <saga::impl::engine>       t_engine_);
         ~task (void);

          void_t                               constructor (void);
          saga::async::state                   get_state   (void);
          saga::util::shared_ptr <result_base> get_result  (void);
          void_t                               run         (void);
          void_t                               wait        (void);

          virtual void dump (std::string msg = "")
          {
            LOGSTR (DEBUG, "task dump") 
              << "impl::task (" << this << ") : " 
              << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
            t_cc_.dump  ();
            t_cc_->dump ();
          }

      }; // class task

    } // namespace async

  } // namespace impl

} // namespace saga

#endif //  SAGA_IMPL_TASK_TASK_HPP

