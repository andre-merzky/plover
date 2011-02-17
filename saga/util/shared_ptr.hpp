
#ifndef SAGA_UTIL_SHAREDPTR_HPP
#define SAGA_UTIL_SHAREDPTR_HPP

#include <saga/util/scoped_lock.hpp>

#include <exception> // for std exception types
#include <iostream>  // for stdout/stderr
#include <typeinfo>  // for std::bad_cast


namespace saga
{
  namespace util
  {
    //////////////////////////////////////////////////////////////////
    //
    // The shared_ptr class is a pointer container which
    // performes reference counting for the pointer it holds.
    // The pointer is deleted when the reference count reaches
    // zero, i.e. when the last copy of the shared pointer is
    // deleted.
    //
    // NOTES:
    //   - the  application must ensure that the pointer is 
    //     not in use outside that reference counting scheme, 
    //     at time of deletion.
    //
    // LIMITATIONS:
    //   - refcounter is of long type, that obviously limits 
    //     the number of allowed copies
    //
    //   - to obtain correct deletion semantics for pointers to
    //     derived classes, the classes involved SHOULD have
    //     virtual destructors.
    //
    //   - needs a reset() and swap() method
    //
    //   - needs something like 'get_shared_from_this <U> ()', which 
    //     allows to create another shared pointer instance, in 
    //     particular one having a different template type, to 
    //     share the same pointer.
    //
    template <class T> class shared_ptr
    {
      // allow for dynamic casts to other shared ptr types
      template <class U> friend class shared_ptr;

      private:
        T                 * ptr_; // holds the pointer
        long              * cnt_; // ref counter
        saga::util::mutex * mtx_; // lock

        // Increment the reference count.  
        void inc_ (void)
        {
          std::cout << " -> inc_ " << count () << " - " << cnt_ << " - " << this << " - " << ptr_ << std::endl;

          saga::util::scoped_lock lck (mtx_);
          (*cnt_) += 1;

          std::cout << " <- inc_ " << count () << std::endl;
        }


        // Decrement the reference count.   If ref count reaches zero, delete
        // the pointer.
        void dec_ (void)
        {
          std::cout << " -> dec_ " << count () << " - " << cnt_ << " - " << this << " - " << ptr_ << std::endl;


          bool delete_mtx = false;

          {
            saga::util::scoped_lock lck (mtx_);
            (*cnt_) -= 1;

            std::cout << " <- dec_ " << count () << std::endl;

            if ( 0 == count () )
            {
              std::cout << " !! del  " << count () << std::endl;

              if ( NULL != ptr_ )
              {
                // not managing a NULL pointer...
                delete (ptr_);
                ptr_ = NULL;
              }

              delete (cnt_);
              cnt_ = NULL;
            }
          }

          if ( delete_mtx )
          {
            // scoped lock got unlocked, we can safely remove the mutex
            // FIXME: check for race conditions with other instances waiting on
            // this mutex
            delete (mtx_);
            mtx_ = NULL;
          }
        }


      public:
        // default ctor
        // marked explicit, so we never automatically convert to a shared pointer.
        // 
        // The default parameter allows us to manage NULL pointers, too, to enable
        //
        //   shared_ptr <int> needed_later;
        //   needed_later = old_shared_ptr;
        //
        explicit shared_ptr (T * p = NULL)
          : ptr_ (p)
          , cnt_ (new long)
          , mtx_ (new saga::util::mutex)
        {
          std::cout << " new counter " << cnt_ << std::endl;

          // init counter
          (*cnt_) = 0;

          // increment the counter
          inc_ ();
        }


        // copy ctor, increments ref count
        shared_ptr (const shared_ptr <T> & that)
          : ptr_           (that.ptr_)
          , cnt_           (that.cnt_)
          , mtx_           (that.mtx_)
        {
          inc_ ();
        }


        // casting copy ctor, increments ref count
        // FIXME: dynamic_cast can throw!
        template <class U> 
        shared_ptr (const shared_ptr <U> & that)
          : ptr_           (dynamic_cast <T*> (that.ptr_))
          , cnt_           (that.cnt_)
          , mtx_           (that.mtx_)
        {
          inc_ ();
        }


        // dtor, decrements ref count
        ~shared_ptr () 
        {
          dec_ ();
        }


        // assignment
        shared_ptr <T> & operator= (const shared_ptr <T> & that) 
        {
          // if we get assigned a new pointer, we need to decrease the ref count
          // of the old one, before we handle the new one.  Otherwise, we do
          // nothing...
          if ( this != & that ) 
          {
            // decrease refcount for the old ptr, and delete if needed
            dec_ ();

            // init 'this' just as in the copy c'tor
            this->ptr_  = that.ptr_;
            this->cnt_  = that.cnt_;
            this->mtx_  = that.mtx_;

            // increment ref count for new pointer
            inc_ ();
          }

          return *this;
        }


        // accessors
        T & operator * ()  const 
        {
          // FIXME: catch NULL ptr_?
          return *ptr_;
        }


        T * operator->() const
        {
          // FIXME: catch NULL ptr_!
          return ptr_;
        }


        // note that the returned pointer is volatile, in the sense that it can
        // be deleted by the shared_ptr d'tor at any time.
        T * get ()  const 
        {
          // FIXME: catch NULL ptr_?
          return ptr_;
        }

        // dyamic_cast'ing version of get
        // remarks to get() apply
        template <class U> 
        U * get (void)
        {
          // FIXME: catch NULL ptr_?
          U * ret;
          
          try
          {
            ret = dynamic_cast <U *> (ptr_);
          }
          catch ( const std::exception & e )
          {
            std::cerr << "bad dynamic_ptr_cast: " << e.what () << std::endl;
            ret = NULL;
          }

          if ( NULL == ret )
          {
            // FIXME: log
            // FIXME: how should we react?  return an empty shared ptr, as we do
            // now, or throw and internal exception?
          }

          return ret;
        }



        // FIXME: this should be a shared_from_this like thing, but is not!
        shared_ptr <T> get_shared_ptr (void)
        {
          // FIXME: catch NULL ptr_
          return (shared_ptr <T> (*this));
        }


        // FIXME: this should be a shared_from_this like thing, but is not!
        template <class U> 
        shared_ptr <U> get_shared_ptr (void)
        {
          // use casting copy c'tor to get new shared ptr
          shared_ptr <U> ret = shared_ptr <U> (*this);

          return ret;
        }


        // get the current refcounter value
        long count ()  const 
        {
          return *cnt_;
        }


        // allow to compare shared pointers, by comparing the contained pointer
        bool compare (const shared_ptr <T> & that) const
        {
          return (ptr_ == that.ptr_);
        }


        // allow simple boolean test, e.g. for if's
        operator bool() const { return (!!ptr_); }
    };


    // == operator as shortcut for a.compare (b)
    template <class T, class U> inline bool operator== (shared_ptr <T> const & a, 
                                                        shared_ptr <U> const & b)
    {
      return a.compare (b);
    }
    

  } // namespace util

} // namespace saga


#endif // SAGA_UTIL_SHAREDPTR_HPP

