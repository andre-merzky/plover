
#ifndef SAGA_CPI_FILESYSTEM_FILE_HPP
#define SAGA_CPI_FILESYSTEM_FILE_HPP

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
    namespace filesystem
    {
      class file_cpi : public saga::impl::cpi_base
      {
        public:
          virtual ~file_cpi (void) 
          {
            SAGA_UTIL_STACKTRACE ();
          };

          virtual void constructor (saga::util::shared_ptr <call_context> cc, 
                                      std::string                           url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            throw "constructor: NotImplemented"; 
          } 

          virtual void get_size (saga::util::shared_ptr <call_context> cc)
          { 
            SAGA_UTIL_STACKTRACE ();
            throw "get_size : NotImplemented"; 
          } 

          //  we have a second get_size method for the async versions.  The
          //  call_mode parameter is somewhat redundant, as the cm is also
          //  stored in the cc, but it allows to use overloading in the adaptor
          //  for the various sync/async calls.  The adaptor needs to switch
          //  over the enum to see what async flavor is wanted / needed, 
          //  but the returned task's state can easily be adjusted by the
          //  calling functor or by the engine.
          virtual void get_size (saga::util::shared_ptr <call_context> cc, 
                                 saga::impl::call_mode                 m)
          { 
            SAGA_UTIL_STACKTRACE ();
            throw "get_size <...> : NotImplemented"; 
          } 

          virtual void copy (saga::util::shared_ptr <call_context> cc, 
                             std::string                           tgt)
          {
            SAGA_UTIL_STACKTRACE ();
            throw "copy : NotImplemented"; 
          }
      };

    } // namespace filesystem

  } // namespace impl

} // namespace saga

#endif //  SAGA_CPI_FILESYSTEM_FILE_HPP

