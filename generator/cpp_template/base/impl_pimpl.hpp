
#ifndef SAGA_BASE_IMPL_PIMPL_HPP
#define SAGA_BASE_IMPL_PIMPL_HPP

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

#include <saga/util/shareable.hpp>
#include <saga/util/shared_ptr.hpp>

#include <iostream>  // for debug messages
#include <typeinfo>  // for better debug messages
#include <string>    // we use strings as exceptions *blush*


namespace saga
{
  ////////////////////////////////////////////////////////////////////////////////
  //
  // impl class pimpl base
  //
  namespace impl
  {
    ////////////////////////////////////////////////////////////////////////////////
    //
    // impl::pimpl is a virtually (pun!) empty class, which serves as polymorphism
    // anchor for all derived impl classes.  That way we can reflect the (here
    // also polymorph) inheritance hierarchy of the facade side, and ensure that
    // dynamic_cast's up and down that hierarchy function as expected.
    //
    // Note that in this example code, polymorphism is used on both sides of 
    // the hierarchy.  That is, depending on the application code, not really
    // required for the pimpl scheme to work.
    //
    class pimpl : public saga::util::shareable
    {
      private:
        // the impl::pimpl class needs to be polymorphic for the up/down casting of
        // its (and its descendent's) pointers to work (via dynamic_cast).  So, we add
        // one virtual function, but hide it.
        virtual void polymorph_ (void) 
        {
        }
  
  
      protected:
        // the c'tor is protected - that way, the only way to create an
        // impl::pimpl instance it to use the static create() method - which
        // returns a shared_pointer!
        pimpl (void) 
        {
        }
  
  
      public:
        // we need a virtual d'tor, too.  Note that all d'tors in derived classes
        // will automatically be virtual now - no need to declare that again
        // (but no harm in doing so either)
        virtual ~pimpl (void) 
        {
        }
  
        // other methods do not need to be virtual (but can be if needed).  In
        // general, impl::pimpl should be kept empty, to make its job (to act as
        // polymorphic impl base) more obvious.
        void impl_test (void) 
        {
        }

        // this is the factory create() method, representing the *only* way to
        // create instances of this pimpl base class.  That way, only shared
        // pointers of this base (and thus of derived classes) can ever exist.
        static saga::util::shared_ptr <impl::pimpl> create (void)
        {
          return saga::util::shared_ptr <impl::pimpl> (new impl::pimpl ());
        }
    };
  
  } // namespace impl
  
} // namespace saga

#endif // SAGA_BASE_IMPL_PIMPL_HPP

