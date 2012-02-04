
#ifndef SAGA_UTIL_SCOPEDLOCK_HPP
#define SAGA_UTIL_SCOPEDLOCK_HPP

#include <saga/util/mutex.hpp>
#include <saga/util/lockable.hpp>

namespace saga 
{
  namespace util
  {

    template <typename T> class shared_ptr;
    class shareable;

    // This class provides a simple scoped lock, based on the
    // saga::util::mutex class.
    class scoped_lock 
    {
      private:
        saga::util::mutex * mtx_;

        // FIXME: forbid copy and assignment etc.
       

      public:
        scoped_lock  (saga::util::mutex * mtx_);
        scoped_lock  (saga::util::lockable & mtx_);
        ~scoped_lock (void); 
    };

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_SCOPEDLOCK_HPP

