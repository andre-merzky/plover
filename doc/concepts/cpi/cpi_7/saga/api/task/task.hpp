
#ifndef SAGA_API_TASK_TASK_HPP
#define SAGA_API_TASK_TASK_HPP

#include <string>

#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/impl/task/task.hpp>

namespace saga
{
  namespace async
  {
    class task
    {
      public:
      private:
        saga::util::shared_ptr <saga::impl::task> impl_;

      public:
        task (void);
        task (saga::util::shared_ptr <saga::impl::task> impl);

        saga::impl::call_state get_state (void);

        template <typename T>
          T get_result (void)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <saga::impl::result_t> result = impl_->get_result (); 
            saga::util::shared_ptr <saga::impl::result_t_detail_ <T> > casted = result.get_shared_ptr <saga::impl::result_t_detail_ <T> > ();
            // FIXME: error check
            LOGSTR (DEBUG, "task get_result") << " 1 ##############################################################" << std::endl;
            LOGSTR (DEBUG, "task get_result") << " ------------------------------------ " << saga::util::demangle (typeid (T).name ()) << std::endl;
            impl_.dump ();
            LOGSTR (DEBUG, "task get_result") << " ------------------------------------ " << std::endl;
            impl_->dump ();
            LOGSTR (DEBUG, "task get_result") << " 2 ##############################################################" << std::endl;

            T t = casted->get ();
            LOGSTR (DEBUG, "task get_result") << " 3 ##############################################################" << std::endl;
            return t;
          }
    };

  } // namespace async

} // namespace saga

#endif //  SAGA_API_TASK_TASK_HPP

