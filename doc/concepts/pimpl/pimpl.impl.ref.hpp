
#ifndef PIMPL_IMPL_REF_HPP
#define PIMPL_IMPL_REF_HPP

// C++ includes
#include <vector>
#include <iostream>

// C includes
#include <assert.h>

// TODO: this convenience-gcc-demangling needs to go, eventually
#if 1
 #include <cxxabi.h>
  const char* demangle(const char* name)
  {
    return __cxxabiv1::__cxa_demangle (name, NULL, 0, NULL);
  }
#else
  const char* demangle(const char* name)
  {
    return name;
  }
#endif


namespace saga
{
  // impl classes
  namespace impl
  {
    // forward declaration
    class refcount;

    // instances of this class are passed around, and copied, and in general take care that the ref
    // counting *within* the impl_ instance (whose pointer it caries and copies around) get
    // correctly updated.  When no ref instances which point to that impl exist anymore, that impl
    // can safely be destroyed.
    //
    // Only impl classes should create instances of ref, giving 'this' to the c'tor
    template <class T>
    class ref
    {
      private:
        T * impl_;

      public:
        ref (T * impl) 
          : impl_ (impl)
        {
          assert ( impl != NULL );

          // how to avoid race that another thread deleted impl at this point, when it still
          // lived when it was passed to the ctor?  Well, that is why it is always created by 'this'
          // - so when the ctor returns, 'this' is sure to still be valid.
          //
          // related: 'this' should never get deleted, but should delete itself when it's rcnt goes to
          // zero.
          impl_->inc ();
          std::cout << "creating new ref to " << demangle (typeid (impl_).name ()) << "\n";
        }

        // copy constructor
        ref (const ref <T> & src) 
        {
          impl_ = src.impl_;
          impl_->inc ();
          std::cout << "copying      ref to " << demangle (typeid (impl_).name ()) << "\n";
        }

        ~ref (void)
        {
          std::cout << "destroying   ref to " << demangle (typeid (impl_).name ()) << "\n";
          impl_->dec ();
        }

        T * get_impl (void) 
        { 
          return impl_; 
        }

        template <class U>
        ref <U> get_ref (void) 
        { 
          return ref <U> (dynamic_cast <U *> (impl_)); 
        }
    };

    // this class is the base for *all* impl objects.  Pointers to that class (or to inherited
    // classes) are those which are supposed to be copied around within a ref instance.
    // TODO: mutex is needed, and should lock all member functions.
    class refcount
    {
      // ref (and derivates, in all T types) are the only classes which can call inc() and dec().
      template <class T> 
      friend class ref;

      private:
        int cnt_;

      protected:
        refcount (void) 
        {
          // we do *not* call inc() here - that is done from the ref container we live in.
          // symetrically, the destructor is not calling dec(), but only checks if cnt_ is indeed
          // zero.
        }

        // destructor is virtual, so that destruction of classes derived from refcount get properly
        // destructed (if d'tor is virtual, d'tors of derived classes are invoked, too).
        virtual ~refcount (void)
        {
          std::cout << "d'tor refcount        () \t- " << this << " - " << cnt_ << "\n";

          // The whole point of this class is that the destructor is never called directly, but
          // only via 'delete this' in dec(), when the counter goes to zero.  Thus, we double 
          // check that the counter is indeed zero, and otherwise complain.
          assert ( cnt_ == 0 );
        }

        // increment the refcount.  This method is ONLY to be called from the ref container, not on
        // this instance directly.  Thus the protection.
        bool inc (void)
        {
          std::cout << "check refcount::inc   () \t- " << this << " - " << cnt_ << "\n";
          cnt_++;
        }

        // decrement the refcount.  This method is ONLY to be called from the ref container, not on
        // this instance directly.  Thus the protection.
        bool dec (void)
        {
          std::cout << "check refcount::dec   () \t- " << this << " - " << cnt_ << "\n";

          cnt_--;
          
          if ( cnt_ == 0 )
          {
            // ATTENTION: the line below ONLY works when the instance was allocated via new!
            // ERGO: impl instances MUST be allocated by new, and MUST NOT be deleted in any other
            // way.  Also, the ref class which called dec() needs to be aware of the fact that this
            // call may have removed the class instance.  The return value is indicating this and
            // MUST be evaluated.  Thus, dec() SHOULD only be called when the calling instance goes
            // out of scope, or gets otherwise destroyed.
            std::cout << "deleting this            \t- " << this << " - " << cnt_ << "\n";
            delete this;
            return false;
          }

          return true;
        }

        // // Note: seems that the entity calling get_ref needs to know the type of the instance it is
        // // called upon.  Is that a problem?
        // // Note: Well, seems we don't need this after all...
        //
        // template <class T> 
        // ref <T> get_ref (void)
        // {
        //   return ref <T> (this);
        // }
    };

    // free function, which creates a ref to a given pointer.  
    // T MUST inherit refcount, otherwise the ref constructor will throw on calling inc()
    template <class T> 
    ref <T> create_ref (T * t)
    {
      return ref <T> (t);
    }

    // // same as above, but return a ref to a different type.  
    // // U MUST be a base class of T, otherwise get_ref will throw on dynamic_cast()
    // // U MUST inherit refcount, otherwise the ref constructor will throw on calling inc()
    // template <class T, class U> 
    // ref <T> create_ref (T * t)
    // {
    //   ref <T> rt (t);
    //   return rt.get_ref <U> ();
    // }

  } // namespace impl

} // namespace saga

#endif // PIMPL_IMPL_REF_HPP

