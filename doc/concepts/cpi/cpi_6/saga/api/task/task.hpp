
#ifndef SAGA_API_TASK_TASK_HPP
#define SAGA_API_TASK_TASK_HPP

#include <map>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <saga/util/mutex.hpp>
#include <saga/util/scoped_lock.hpp>
#include <saga/util/shareable.hpp>
#include <saga/util/enums.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/cpi/task/task.hpp>

namespace saga
{
  class task
  {
    private:
      saga::util::shared_ptr <saga::impl::task> impl_;

    public:
      task (void)
        : impl_ (new saga::impl::task)
      {
        SAGA_UTIL_STACKTRACE ();
        (void) impl_->constructor ();
      }

      task (saga::util::shared_ptr <saga::impl::task> impl)
        : impl_ (impl)
      {
        SAGA_UTIL_STACKTRACE ();
        (void) impl_->constructor ();
      }

      saga::impl::call_state get_state (void)
      {
        SAGA_UTIL_STACKTRACE ();
        return impl_->get_state ();
      }

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

} // namespace saga

#endif //  SAGA_API_TASK_TASK_HPP

