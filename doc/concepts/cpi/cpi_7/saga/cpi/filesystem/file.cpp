
#include "file.hpp"

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
      file_cpi::~file_cpi (void) 
      {
        SAGA_UTIL_STACKTRACE ();
      }

      void file_cpi::constructor (saga::util::shared_ptr <call_context> cc, 
                                  std::string                           url) 
      { 
        SAGA_UTIL_STACKTRACE ();
        throw "constructor: NotImplemented"; 
      } 

      void file_cpi::get_size (saga::util::shared_ptr <call_context> cc)
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
      void file_cpi::get_size (saga::util::shared_ptr <call_context> cc, 
                              saga::async::mode                      m)
      { 
        SAGA_UTIL_STACKTRACE ();
        throw "get_size <...> : NotImplemented"; 
      } 

      void file_cpi::copy (saga::util::shared_ptr <call_context> cc, 
                                   std::string                           tgt)
      {
        SAGA_UTIL_STACKTRACE ();
        throw "copy : NotImplemented"; 
      }
    } // namespace filesystem

  } // namespace impl

} // namespace saga

