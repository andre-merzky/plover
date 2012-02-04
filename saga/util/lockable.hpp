
#ifndef SAGA_UTIL_LOCKABLE_HPP
#define SAGA_UTIL_LOCKABLE_HPP

#include <saga/util/mutex.hpp>

namespace saga 
{
  namespace util
  {
    // This class can serve as a decorator for classes which need to be
    // locked/unlocked frequently.
    class lockable 
    {
      private:
        saga::util::mutex * mtx_;
        bool locked_;
         
        // forbid copy -- well, the mtx_ cannot be copied anyway...
        lockable             (      lockable & m) { throw "lockable copy construction is not allowed!"; } 
        lockable             (const lockable & m) { throw "lockable copy construction is not allowed!"; } 
        lockable & operator= (      lockable & m) { throw "lockable copy assignment   is not allowed!"; }
        lockable & operator= (const lockable & m) { throw "lockable copy assignment   is not allowed!"; }


      public:
        lockable (saga::util::mutex * mtx)
          : mtx_ (mtx)
          , locked_ (false)
        {
        }

        lockable (void)
          : mtx_ (new saga::util::mutex)
          , locked_ (false)
        {
        }

        ~lockable () 
        {
          if ( mtx_ )
            delete (mtx_);
        }

        void lock (void) 
        {
          mtx_->lock ();
          locked_ = true;
        }

        void unlock (void) 
        {
          locked_ = false;
          mtx_->unlock ();
        }

        bool locked (void)
        {
          return locked_;
        }

        bool try_lock (void)
        {
          return mtx_->try_lock ();
        }

        void set_mutex (saga::util::mutex * mtx)
        {
          // FIXME: do we really understand the implications here?
          mtx_ = mtx;
        }

        saga::util::mutex * get_mutex (void)
        {
          // FIXME: now, this is dangerous: we hand out refs to our private
          // mutex, for example to a scoped lock, but have no means to make sure
          // that we don't delete it before that scoped lock releases it...
          // It is thus up to the callee of this function to ensure proper
          // resource management
          return mtx_;
        }
    };

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_MUTEX_HPP

