
#ifndef SAGA_API_ASYNC_TASK_HPP
#define SAGA_API_ASYNC_TASK_HPP

#include <string>

#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/api/async/state.hpp>

#include <saga/impl/async/task.hpp>

#define TASK_DELAY 5000

namespace saga
{
  namespace async
  {
    class task
    {
      private:
        saga::util::shared_ptr <saga::impl::async::task> impl_;

      public:
        task (void);
        task (saga::util::shared_ptr <saga::impl::async::task> impl);

        saga::async::state get_state (void);
        void               run       (void);
        void               wait      (void);

        template <typename T>
        T get_result (void)
        {
          SAGA_UTIL_STACKTRACE ();

          // FIXME: error check: is cast valid?
          LOGSTR (DEBUG, "saga::async::task get_result") << " getting task result" << std::endl;

          return impl_->get_result ()->get <T> (); 
        }
    };

  } // namespace async

} // namespace saga

#endif //  SAGA_API_ASYNC_TASK_HPP

