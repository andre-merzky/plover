
#include <saga/util/scoped_lock.hpp>
#include <saga/util/shareable.hpp>
#include <saga/util/mutex.hpp>

namespace saga 
{
  namespace util
  {
    scoped_lock::scoped_lock (saga::util::mutex * m)
      : mtx_ (m)
    {
      mtx_->lock ();
    }

    scoped_lock::scoped_lock (saga::util::lockable & l)
      : mtx_ (l.get_mutex ())
    {
      mtx_->lock ();
    }

    scoped_lock::~scoped_lock () 
    {
      mtx_->unlock ();
    }

  } // namespace util

} // namespace saga

