
#ifndef SAGA_UTIL_SCOPEDLOCK_HPP
#define SAGA_UTIL_SCOPEDLOCK_HPP

#include <saga/util/mutex.hpp>

namespace saga 
{
  namespace util
  {
    // This class provides a simple scoped lock, based on the
    // saga::util::mutex class.
    class scoped_lock 
    {
      private:
        saga::util::mutex mtx_;

      public:
        scoped_lock (void)
        {
          mtx_.lock ();
        }

        ~scoped_lock () 
        {
          mtx_.unlock ();
        }
    };

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_SCOPEDLOCK_HPP

