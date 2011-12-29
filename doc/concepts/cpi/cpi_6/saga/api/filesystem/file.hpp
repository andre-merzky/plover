
#ifndef SAGA_API_FILESYSTEM_FILE_HPP
#define SAGA_API_FILESYSTEM_FILE_HPP

// FIXME: clean out
#include <map>
#include <vector>

// FIXME: clean out
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// FIXME: clean out, add engine
#include <saga/util/mutex.hpp>
#include <saga/util/scoped_lock.hpp>
#include <saga/util/shareable.hpp>
#include <saga/util/enums.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/cpi/file/file.hpp>

namespace saga
{
  namespace filesystem
  {
    class file
    {
      private:
        saga::util::shared_ptr <saga::impl::filesystem::file> impl_;

      public:
        file (std::string url)
          : impl_ (new saga::impl::filesystem::file)
        {
          SAGA_UTIL_STACKTRACE ();
          (void) impl_->constructor (url);
        }

        file (saga::util::shared_ptr <saga::impl::filesystem::file> impl)
          : impl_ (impl)
        {
          SAGA_UTIL_STACKTRACE ();
          // FIXME: where to get the URL/idata?  Is constructor here really useful,
          // as impl already exists?  Is impl always initialized?
          // In general, need to clear up who is filling idata, when the object
          // is adaptor initialized / bound (void) 
          //
          // impl_->constructor (url);
        }

        int get_size (void)
        {
          SAGA_UTIL_STACKTRACE ();
          return impl_->get_size ();
        }

        template <enum saga::impl::call_mode M>
        saga::task get_size (void)
        {
          SAGA_UTIL_STACKTRACE ();
          return saga::task (impl_->get_size (M));
        }

        void copy (std::string tgt)
        {
          SAGA_UTIL_STACKTRACE ();
          (void) impl_->copy (tgt);
        }
    };

  } // namespace filesystem

} // namespace saga

#endif //  SAGA_API_FILESYSTEM_FILE_HPP

