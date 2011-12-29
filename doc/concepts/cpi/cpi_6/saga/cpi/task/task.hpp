
#ifndef SAGA_CPI_TASK_TASK_HPP
#define SAGA_CPI_TASK_TASK_HPP

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

namespace saga
{
  namespace impl
  {
    class task_cpi : public saga::impl::cpi_base
    {
      public:
        virtual ~task_cpi (void) { };

        virtual void constructor (saga::util::shared_ptr <call_context> cc)
        { 
          SAGA_UTIL_STACKTRACE ();
          throw "constructor: NotImplemented"; 
        } 

        virtual void get_state (saga::util::shared_ptr <call_context> cc)
        {
          SAGA_UTIL_STACKTRACE ();
          throw "get_state : NotImplemented"; 
        }

        // FIXME: cont here (any needed)
        virtual void get_result (saga::util::shared_ptr <call_context> cc)
        { 
          SAGA_UTIL_STACKTRACE ();
          throw "get_result : NotImplemented"; 
        } 
    };

  } // namespace impl

} // namespace saga

#endif //  SAGA_CPI_TASK_TASK_HPP

