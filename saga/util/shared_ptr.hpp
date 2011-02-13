
#ifndef SAGA_UTIL_SHAREDPTR_HPP
#define SAGA_UTIL_SHAREDPTR_HPP

#include <saga/util/scoped_lock.hpp>

#include <exception> // for std exception types
#include <iostream>  // for stdout/stderr

namespace saga
{
  namespace util
  {
    //////////////////////////////////////////////////////////////////
    //
    // the shared_ptr class is a pointer container which
    // performes reference counting for the pointer it holds.
    // The pointer is deleted whn the reference count reaches
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
        T     * ptr_;  // holds the pointer
        long  * cnt_;  // reference count


        // Increment the reference count.  
        // The counter is protected by a scoped lock.
        void inc_ (void)
        {
          saga::util::scoped_lock lck;
          (*cnt_) += 1;
        }


        // Decrement the reference count.   If ref count reaches zero, delete
        // the pointer.
        // The counter is protected by a scoped lock.
        void dec_ (void)
        {
          saga::util::scoped_lock lck;

          (*cnt_) -= 1;

          if ( 0 == (*cnt_) )
          {
            delete (cnt_);
            delete (ptr_);

            ptr_ = NULL;
            cnt_ = NULL;
          }
        }


      public:
        // default ctor
        // marked explicit, so we never automatically convert to a shared pointer
        explicit shared_ptr ( T * p = 0 )
          : ptr_ (p)
        {
          try 
          {
            // get new counter, and increment
            cnt_ = new long;
            inc_ ();
          }
          catch ( ... )
          {
            // can't do much if we don't even get a counter allocated
            delete (p);
            throw; // FIXME: throw something sensible
          }
        }


        // copy ctor, increments ref count
        shared_ptr (const shared_ptr <T> & p)
          : ptr_ (p.ptr_), 
            cnt_ (p.cnt_)
        {
          inc_ ();
        }


        // casting copy ctor, increments ref count
        // FIXME: dynamic_cast can throw!
        template <class U> 
        shared_ptr (const shared_ptr <U> & p)
          : ptr_ (dynamic_cast <T*> (p.ptr_)), 
            cnt_ (p.cnt_)
        {
          inc_ ();
        }


        // dtor, decrements ref count
        ~shared_ptr () 
        {
          dec_ ();
        }


        // assignment
        // FIXME: do we need locking here?  What if someone else decrements
        // between the dec/inc below, for the same pointer?  It will be gone
        // before the inc...
        shared_ptr <T> & operator= (const shared_ptr <T> & p) 
        {
          // really got  a new pointer?
          if ( this != & p ) 
          {
            // decrease refcount for the old ptr, and delete if needed
            dec_ ();
          }

          // init 'this' just as in the copy c'tor
          ptr_ = p.ptr_;
          cnt_ = p.cnt_;

          // increment ref count for new pointer
          inc_ ();

          return *this;
        }


        // accessors
        T & operator * ()  const 
        {
          return *ptr_;
        }


        T * operator->() const
        {
          return ptr_;
        }


        // note that the returned pointer is volatile, in the sense that it can
        // be deleted by the shared_ptr d'tor at any time.
        T * get ()  const 
        {
          return ptr_;
        }

        // dyamic_cast'ing version of get
        // remarks to get() apply
        template <class U> 
        U * get (void)
        {
          U * ret;
          
          try
          {
            ret = dynamic_cast <U *> (ptr_);
          }
          catch ( const std::exception & e )
          {
            std::cerr << "bad dynamic_ptr_cast: " << e.what () << std::endl;
            return NULL;
          }

          return ret;
        }



        // allow to compare shared pointers, by comparing the contained pointer
        bool compare (const shared_ptr <T> & p) const
        {
          return (ptr_ == p.ptr_);
        }


        // FIXME: this should be a shared_from_this like thing, but is not!
        template <class U> 
        shared_ptr <U> dynamic_pcast (void)
        {
          U * tmp;

          try 
          {
            tmp = dynamic_cast <U *> (ptr_);
          }
          catch ( const std::exception & e )
          {
            std::cerr << "bad dynamic_shared_ptr_cast: " << e.what () << std::endl;
            tmp = NULL;
          }

          return shared_ptr <U> (tmp);
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

