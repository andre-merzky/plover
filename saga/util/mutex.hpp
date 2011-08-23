
#ifndef SAGA_UTIL_MUTEX_HPP
#define SAGA_UTIL_MUTEX_HPP

#include <iostream>
#include <pthread.h>

// FIXME: check mutex call return values

namespace saga 
{
  namespace util
  {
    // This class provides a wrapper around a pthread mutex's, and allows to
    // lock/unlock them.  The class should be exception save, i.e. the mutex
    // gets destroyed if the classes destructor is called.  That implies that no
    // copy constructor can be available - otherwise one copy could destroy
    // a mutex needed by another copy
    class mutex 
    {
      private:
        pthread_mutex_t mtx_;
         
        // forbid copy
        mutex (mutex & that) 
        {
          // we must not be called
          throw;  // FIXME: throw something sensible
        }

        mutex (const mutex & m)
        {
          // we must not be called
          throw;  // FIXME: throw something sensible
        }


        // FIXME: define copy, assignment etc, and only allow for unlocked
        // mutexes?




      public:
        mutex (void)
        {
          pthread_mutexattr_t         attr;
          pthread_mutexattr_init    (&attr);
          pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_ERRORCHECK);
          pthread_mutex_init (&mtx_, &attr);

          // std::cout << " ===== mtx ctor " << this << std::endl;
        }

        ~mutex () 
        {
          // std::cout << " ===== mtx dtor " << this << std::endl;
          pthread_mutex_destroy (&mtx_);
        }

        void lock (void) 
        {
          // std::cout << " ===== mtx lock " << this << std::endl;
          pthread_mutex_lock (&mtx_);
        }

        void unlock (void) 
        {
          // std::cout << " ===== mtx unlock " << this << std::endl;
          pthread_mutex_unlock (&mtx_);
        }

        bool try_lock (void)
        {
          // std::cout << " ===== mtx tried " << this << " : " << pthread_mutex_trylock (&mtx_) << std::endl;

          if ( 0 == pthread_mutex_trylock (&mtx_) )
          {
            return true;
          }

          return false;
        }

        mutex * get_mutex (void)
        {
          // std::cout << " ===== mtx get " << this << std::endl;
          return this;
        }
    };

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_MUTEX_HPP

