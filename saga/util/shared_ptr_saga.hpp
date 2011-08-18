
#ifndef SAGA_UTIL_SHAREDPTRSAGA_HPP
#define SAGA_UTIL_SHAREDPTRSAGA_HPP

#ifdef SAGA_USE_TR1
# warning "this include should only be used with TR1 undefined"
#endif

#include <saga/util/demangle.hpp>
#include <saga/util/scoped_lock.hpp>

#include <exception> // for std exception types
#include <iostream>  // for stdout/stderr
#include <typeinfo>  // for std::bad_cast


namespace saga
{
  namespace util
  {
    // forward declaration
    class shareable;

    //////////////////////////////////////////////////////////////////
    //
    // The shared_ptr class is a pointer container which
    // performes reference counting for the pointer it holds.
    // The pointer is deleted when the reference count reaches
    // zero, i.e. when the last copy of the shared pointer is
    // deleted.
    //
    //
    // NOTES:
    //   - the  application must ensure that the pointer is 
    //     not in use outside that reference counting scheme, 
    //     at time of deletion.
    //
    //   - to support that, the get() and get<U>() methods 
    //     are private - the user should only be able to get 
    //     new shared pointers.
    //
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
          saga::util::scoped_lock lck (mtx_);
          (*cnt_) += 1;
          // std::cout << " === inc     : " << get_ptype_demangled() << " - " << ptr_ << " - " << (*cnt_) << std::endl;
        }


        // Decrement the reference count.   If ref count reaches zero, delete
        // the pointer.
        void dec_ (void)
        {
          bool delete_mtx = false;

          {
            saga::util::scoped_lock lck (mtx_);
            (*cnt_) -= 1;

            // std::cout << " === dec     : " << get_ptype_demangled() << " - " << ptr_ << " - " << (*cnt_) << std::endl;

            if ( 0 == get_count () )
            {

              if ( is_a <saga::util::shareable> () )
              {
                get <saga::util::shareable> ()->unshare ();
              }

              if ( NULL != ptr_ )
              {
                // std::cout << " === delete  : " << get_ptype_demangled() << " - " << ptr_ << " - " << (*cnt_) << std::endl;
                delete (ptr_);
                ptr_ = NULL;
              }

              delete (cnt_);
              cnt_ = NULL;

              delete_mtx = true;
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


        // note that the returned pointer is volatile, in the sense that it can
        // be deleted by the shared_ptr d'tor at any time.
        T * get ()  const 
        {
          // FIXME: catch NULL ptr_?
          return ptr_;
        }

        // dyamic_cast'ing version of get.
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
            // FIXME: how should we react?  return an empty ptr, as we do
            // now, or throw an internal exception, or incorrect_type exception?
          }

          return ret;
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
        {
          if ( NULL != ptr_ ) ptr_->is_shareable ();

          mtx_ = new saga::util::mutex;

          // init counter
          (*cnt_) = 0;

          // increment the counter
          inc_ ();

          if ( is_a <saga::util::shareable> () && ! ptr_->is_shared () )
          {
            // std::cout << " === share 1 : " << ptr_ << " - " << cnt_ << " - " << mtx_ << std::endl;
            get <saga::util::shareable> ()->share (mtx_, cnt_);
          }
        }


        // copy ctor, increments ref count
        shared_ptr (const shared_ptr <T> & that)
          : ptr_           (that.ptr_)
          , cnt_           (that.cnt_)
          , mtx_           (that.mtx_)
        {
          if ( NULL != ptr_ ) ptr_->is_shareable ();

          if ( is_a <saga::util::shareable> () && ! ptr_->is_shared () )
          {
            // std::cout << " === share 2" << ptr_ << " - " << std::endl;
            get <saga::util::shareable> ()->share (mtx_, cnt_);
          }

          inc_ ();
        }


        // explicit ctor, uses existing ptr, cnt and mtx, increments ref count
        shared_ptr (saga::util::mutex * mtx, long * cnt, T * ptr)
          : ptr_           (ptr)
          , cnt_           (cnt)
          , mtx_           (mtx)
        {
          if ( NULL != ptr_ ) ptr_->is_shareable ();

          if ( is_a <saga::util::shareable> () && ! ptr_->is_shared () )
          {
            // std::cout << " === share 3" << ptr_ << " - " << std::endl;
            get <saga::util::shareable> ()->share (mtx_, cnt_);
          }

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
          if ( NULL != ptr_ ) ptr_->is_shareable ();

          if ( is_a <saga::util::shareable> () && ! ptr_->is_shared () )
          {
            // std::cout << " === share 4" << ptr_ << " - " << std::endl;
            get <saga::util::shareable> ()->share (mtx_, cnt_);
          }

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

            if ( NULL != ptr_ ) ptr_->is_shareable ();

            if ( is_a <saga::util::shareable> () && ! ptr_->is_shared () )
            {
              // std::cout << " === share 5" << ptr_ << " - " << std::endl;
              get <saga::util::shareable> ()->share (mtx_, cnt_);
            }

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


        shared_ptr <T> get_shared_ptr (void)
        {
          // FIXME: catch NULL ptr_
          // use copy c'tor to get new shared ptr
          return (shared_ptr <T> (*this));
        }


        template <class U> 
        shared_ptr <U> get_shared_ptr (void)
        {
          // FIXME: use casting copy c'tor to get new shared ptr
          U* u = get <U> ();
          shared_ptr <U> ret = shared_ptr <U> (mtx_, cnt_, u);

          return ret;
        }

        // is_a() is related to get_shared_ptr<U>, as it checks if get_shared_ptr<U>, and in
        // particular the casting, would succeed.
        template <typename U>
        bool is_a (void)              
        { 
          // obvious special case
          if ( NULL == ptr_ )
          {
            return false;
          }

          // use casting get to get a U* typed ptr
          U * test = get <U> ();

          if ( NULL == test )
          {
            // no valid ptr cast, so ptr_ is not a U.
            return false;
          }

          // got a valid ptr
          return true;
        }



        // allow to compare shared pointers, by comparing the contained pointer
        bool compare (const shared_ptr <T> & that) const
        {
          return (ptr_ == that.ptr_);
        }


        // allow simple boolean test, e.g. for if's
        operator bool() const { return (!!ptr_); }


        //////////////////////////////////////////////////////////////Q
        //
        // some inspection methods
        //

        // get the current refcounter value
        long get_count ()  const 
        {
          return *cnt_;
        }

        // get the typename of the stored pointer
        std::string get_ptype (void)
        {
          return ptr_->get_ptype ();
        }

        // get the typename of the stored pointer, demangled
        std::string get_ptype_demangled (void)
        {
          return ptr_->get_ptype_demangled ();
        }
    };


    // == operator as shortcut for a.compare (b)
    template <class T, class U> inline bool operator== (shared_ptr <T> const & a, 
                                                        shared_ptr <U> const & b)
    {
      return a.compare (b);
    }

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_SHAREDPTRSAGA_HPP

