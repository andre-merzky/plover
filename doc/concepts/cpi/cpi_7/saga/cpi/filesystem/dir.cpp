
#include "dir.hpp"

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
      dir_cpi::~dir_cpi (void) 
      {
        SAGA_UTIL_STACKTRACE ();
      }

      void dir_cpi::constructor (saga::util::shared_ptr <call_context> cc, 
                                 std::string                           url) 
      { 
        SAGA_UTIL_STACKTRACE ();
        throw "constructor: NotImplemented"; 
      } 

      void dir_cpi::get_url (saga::util::shared_ptr <call_context> cc)
      { 
        SAGA_UTIL_STACKTRACE ();
        throw "get_url : NotImplemented"; 
      } 

      void dir_cpi::open (saga::util::shared_ptr <call_context> cc, 
                          std::string                           url)
      {
        SAGA_UTIL_STACKTRACE ();
        throw "open : NotImplemented"; 
      }

    } // namespace filesystem

  } // namespace impl

} // namespace saga
