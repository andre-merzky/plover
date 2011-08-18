
#ifndef SAGA_UTIL_MUTEX_HPP
#define SAGA_UTIL_MUTEX_HPP

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
          pthread_mutex_init (&mtx_, NULL);
        }

        ~mutex () 
        {
          pthread_mutex_destroy (&mtx_);
        }

        void lock (void) 
        {
          pthread_mutex_lock (&mtx_);
          // std::cerr << "mutex locked\n";
        }

        void unlock (void) 
        {
          pthread_mutex_unlock (&mtx_);
          // std::cerr << "mutex unlocked\n";
        }

        bool try_lock (void)
        {
          if ( 0 == pthread_mutex_trylock (&mtx_) )
          {
            return true;
          }

          return false;
        }

        mutex * get_mutex ()
        {
          return this;
        }
    };

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_MUTEX_HPP
