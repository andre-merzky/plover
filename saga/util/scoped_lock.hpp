
#ifndef SAGA_UTIL_SCOPEDLOCK_HPP
#define SAGA_UTIL_SCOPEDLOCK_HPP

#include <saga/util/mutex.hpp>

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
        bool                own_; // do we own the mutex?

        // FIXME: forbid copy and assignment etc.

      public:
        scoped_lock (saga::util::mutex * mtx);
        scoped_lock (saga::util::shared_ptr <saga::util::shareable> l);
        scoped_lock (saga::util::mutex & mtx); // FIXME: rethink if this one makes sense...
        scoped_lock (void);                    // FIXME: is this version ever useful?
        ~scoped_lock (void); 
    };

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_SCOPEDLOCK_HPP

