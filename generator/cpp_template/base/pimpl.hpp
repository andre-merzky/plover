
#ifndef SAGA_BASE_PIMPL_HPP
#define SAGA_BASE_PIMPL_HPP

////////////////////////////////////////////////////////////////////////////////
//
// SharedPIMPL for Polymorph Class Hierarchies 
// -------------------------------------------
//
// This is the same example as shown in doc_pimple_multiple_inheritance.cpp, but
// instead of a PIMPL (Pointer to IMPLementation) we keep a SharedPIMPL (Shared
// Pointer to implementation),  which alleviates many of the memory management
// and thread safety issues of the pure PIMPL approach.
//
// When using shared pointers, one has to ensure that no pointer instances are
// used outside of shared pointers.  This document shows how to prevent that:
// the impl class constructors are protected, and can thus not be called by
// the user, nor by the facade.  Instead, static create() methods call the
// c'tors, and immediately wrap the new pointer into a shared pointer.  There
// exists no direct accessor for the pointer in the shared pointer class, so
// there is no way that the pointer can be used outside this reference counting
// scheme.
//

#include <saga/base/impl_pimpl.hpp> // the impl part of the pimpl scheme

namespace saga
{

  ////////////////////////////////////////////////////////////////////////////////
  //
  // facade classes pimpl base
  //
  //
  // The facade end of the pimpl mechanism needs to maintain a pointer to the impl
  // classes somewhere.  That is done by this pimpl base class.  That very impl
  // pointer is then used by *all* derived classes to invoke the actual methods on
  // that impl pointer.  The impl pointer must thus be of the type of the most
  // derived class (i.e. must be a context_impl* for a context facade class).
  // That class passes the impl pointer created on construction time to the pimpl
  // base class for storage.  
  //
  // Anyway, once the impl pointer is needed by the facade class, or by any other 
  // class in the facade's inheritance tree, the pointer is retrieved again from 
  // the pimpl base class, and dynamic_cast'ed back into its appropriate type.
  //
  // This pimpl base class is not an actual part of the API!  One could trick
  // around to forbid users to directly create instances, but that is left out for
  // clarity.
  // 
  class pimpl 
  {
    protected:
      // enum to tag c'tors without impl pointer.  
      // Only visible to derived classes.
      enum noimpl_enum
      {
        NO_IMPL = 0
      };
  
  
    private:
      // here we keep the pointer to the impl instance, cast to it's base type
      saga::util::shared_ptr <impl::pimpl> impl_;

      // make our class hierarchy polymorph. (This method can in principle be
      // removed - the pimpl paradigm works just as well on non-polymorph
      // facades.)
      virtual void polymorph_ (void) 
      {
      }
  
  
    protected:
      // we don't want to expose the c'tors of this class on API level - the end
      // user should not need to explicitly create any pimpl instance.
  
      // the first c'tor is explicitly *not* getting an impl pointer: in the case 
      // of multiple inheritance, only one of the classes on any level of the 
      // class hierarchy will pass down the impl pointer - all others will invoke 
      // this empty c'tor for their base class.
      explicit pimpl (noimpl_enum) 
      {
        // if the facade hierarchy is spanned correctly, the following test
        // should work already!  Reason is that before the no_impl c'tor of this
        // base class is getting invoked, the derived class has already invoked 
        // the pimpl c'tor, and has thus already stored a valid impl pointer in
        // the pimpl base class.
        impl_test (); 
      }
  
      // the second c'tor is the one which gets passed down an impl pointer, which
      // is then stored for later usage.  Note that the storage type is the base
      // pointer of the impl hierarchy, but the passed pointer type is (likely)
      // a derived class in the impl hierarchy.
      pimpl (saga::util::shared_ptr <impl::pimpl>  impl) 
        : impl_ (impl) 
      {
        // impl_test ();  // impl should be valid
      }
  
  
    public:
      // ~pimpl does not really delete the impl pointer, but, as that is
      // a shared ptr, will decrease its reference count.
      virtual ~pimpl (void)
      {
        // impl_test ();
      }
  
      // the get_impl() method is very central to our pimpl concept: it allows
      // to cast the impl pointer, which is stored in our virtual pimpl base
      // class, to be cast up and down the impl object hierarchy, so that the
      // respective impl method calls can be performed.
      //
      // Note that the dynamic_cast comes with a certain runtime penalty, as it
      // performs type checking at run time.  If that is starting to hurt your
      // program, you may consider to cache the returned and casted impl
      // pointers either here, or in your derived class.  The complexity will
      // increase however as one then needs to check for dangling pointers...
      // A rough performance measurements yields about 600.000 casts per second
      // (Intel Core i7 M 620 @ 2.67GHz, 4GB, single thread, very small
      // std-deviation)
      //
      template <typename T>
      saga::util::shared_ptr <T> get_impl (void)              
      { 
        // ensure we actually have an impl pointer.  This avoids an exception on
        // the cast later on.
        if ( ! impl_ )
        {
          throw std::string (" pimpl::get_impl<T>() on NULL impl pointer!");
        }
  
        // try to cast to the wanted pointer type
        saga::util::shared_ptr <T> ret = impl_.get_shared_ptr <T> (); 
  
        // if that fails, complain
        if ( ! ret )
        {
          throw std::string (" pimpl::get_impl <")
                  + saga::util::demangle (typeid (T).name ())
                  + "> () cannot cast an " 
                  + impl_.get_ptype_demangled ()
                  + " impl pointer!";
        }
  
        // otherwise return the thus casted pointer.
        // Note that we hand out a second pointer to the same impl instance,
        // with the obvious potential for problems for consistency (double delete, 
        // access after delete, etc)
        return (ret);
      }
  
  
      // is_a<T> is now provided by the shared_ptr impl
      template <typename T>
      bool is_a (void)              
      { 
        return (impl_.is_a <T> ());
      }
  
      // just a test method, which also demonstrated the use of get_impl<T>().
      void impl_test (void) 
      {
        std::cout << " pimpl test" << std::endl; 
        if ( impl_.is_a <impl::pimpl>   () )   get_impl <impl::pimpl>   ()->impl_test    ();
        if ( impl_.is_a <impl::object>  () )   get_impl <impl::object>  ()->object_test  ();
        if ( impl_.is_a <impl::attribs> () )   get_impl <impl::attribs> ()->attribs_test ();
        if ( impl_.is_a <impl::context> () )   get_impl <impl::context> ()->context_test ();
        std::cout << " pimpl test" << std::endl; 
      }
  };
  
} // namespace saga

#endif // SAGA_BASE_PIMPL_HPP

