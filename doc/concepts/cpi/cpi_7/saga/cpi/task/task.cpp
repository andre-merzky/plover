
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include "task.hpp"


namespace saga
{
  namespace impl
  {
    task_cpi::~task_cpi (void) 
    {
      SAGA_UTIL_STACKTRACE ();
    };

    void task_cpi::constructor (saga::util::shared_ptr <call_context> cc)
    { 
      SAGA_UTIL_STACKTRACE ();
      throw "constructor: NotImplemented"; 
    } 

    void task_cpi::get_state (saga::util::shared_ptr <call_context> cc)
    {
      SAGA_UTIL_STACKTRACE ();
      throw "get_state : NotImplemented"; 
    }

    // FIXME: cont here (any needed)
    void task_cpi::get_result (saga::util::shared_ptr <call_context> cc)
    { 
      SAGA_UTIL_STACKTRACE ();
      throw "get_result : NotImplemented"; 
    } 

  } // namespace impl

} // namespace saga
