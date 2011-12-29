
#ifndef SAGA_CPI_FILESYSTEM_DIR_HPP
#define SAGA_CPI_FILESYSTEM_DIR_HPP

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
      class dir_cpi : public saga::impl::cpi_base
      {
        public:
          virtual ~dir_cpi (void) 
          {
            SAGA_UTIL_STACKTRACE ();
          };

          virtual void constructor (saga::util::shared_ptr <call_context> cc, 
                                    std::string                           url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            throw "constructor: NotImplemented"; 
          } 

          virtual void get_url (saga::util::shared_ptr <call_context> cc)
          { 
            SAGA_UTIL_STACKTRACE ();
            throw "get_url : NotImplemented"; 
          } 

          virtual void open (saga::util::shared_ptr <call_context> cc, 
                             std::string                           url)
          {
            SAGA_UTIL_STACKTRACE ();
            throw "open : NotImplemented"; 
          }
      };

    } // namespace filesystem

  } // namespace impl

} // namespace saga

#endif //  SAGA_CPI_FILESYSTEM_DIR_HPP

