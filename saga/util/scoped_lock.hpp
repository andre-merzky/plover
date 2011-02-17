
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
        saga::util::mutex * mtx_;
        bool                own_; // do we own the mutex?

        // FIXME: forbid copy and assignment etc.

      public:
        scoped_lock (saga::util::mutex * mtx)
          : mtx_ (mtx)
          , own_ (false)
        {
          mtx_->lock ();
        }

        scoped_lock (void)
          : mtx_ (new saga::util::mutex ())
          , own_ (true)
        {
          mtx_->lock ();
        }

        ~scoped_lock () 
        {
          mtx_->unlock ();

          if ( own_ )
          {
            delete (mtx_);
          }
        }
    };

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_SCOPEDLOCK_HPP

