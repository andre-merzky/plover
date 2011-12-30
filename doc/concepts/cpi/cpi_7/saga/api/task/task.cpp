
#include "task.hpp"

#include <string>

#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

namespace saga
{
  task::task (void)
    : impl_ (new saga::impl::task)
  {
    SAGA_UTIL_STACKTRACE ();
    (void) impl_->constructor ();
  }

  task::task (saga::util::shared_ptr <saga::impl::task> impl)
    : impl_ (impl)
  {
    SAGA_UTIL_STACKTRACE ();
    (void) impl_->constructor ();
  }

  saga::impl::call_state task::get_state (void)
  {
    SAGA_UTIL_STACKTRACE ();
    return impl_->get_state ();
  }
} // namespace saga

