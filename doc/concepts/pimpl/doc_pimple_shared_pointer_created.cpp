
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


////////////////////////////////////////////////////////////////////////////////
//
// impl classes
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
      // the c'tor is protected - that way, the only means to create an
      // impl::pimpl instance it to use the static create() method - which
      // returns a shared_pointer!
      pimpl (void) { std::cout << " impl    pimpl   c'tor" << std::endl; }


    public:
      // we need a virtual d'tor, too.  Note that all d'tors in derived classes
      // will automatically be virtual now - no need to declare that again, but
      // no problem in doing so either...
      virtual ~pimpl (void) 
      {
        std::cout << " impl    pimpl   d'tor" << std::endl; 
      }

      // other methods do not need to be virtual (but can be if needed).  In
      // general though, impl::pimpl should be kept empty, to make its job (to 
      // act as polymorphic impl base) more obvious.
      void impl_test (void) 
      {
        std::cout << " impl    pimpl   test" << std::endl; 
      }

      // the static create method acts as factory for class instances, and
      // ensures that in fact only shared pointers to class instances ever
      // exist.
      static saga::util::shared_ptr <impl::pimpl> create (void)
      {
        return saga::util::shared_ptr <impl::pimpl> (new impl::pimpl ());
      }
  };


  ////////////////////////////////////////////////////////////////////////////////
  // other impl classes *must*, directly or indirectly, inherit impl::pimpl as
  // 'virtual public base class', to profit from its polymorphism properties.
  class object  : public virtual impl::pimpl 
  {
    protected:
      object           (void) { std::cout << " impl    object  c'tor" << std::endl; }


    public:
      ~object          (void) { std::cout << " impl    object  d'tor" << std::endl; }
      void object_test (void) { std::cout << " impl    object  test"  << std::endl; }

      static saga::util::shared_ptr <impl::object> create (void)
      {
        return saga::util::shared_ptr <impl::object> (new impl::object ());
      }
  };


  ////////////////////////////////////////////////////////////////////////////////
  class attribs : public virtual impl::pimpl 
  {
    protected:
      attribs           (void) { std::cout << " impl    attribs c'tor" << std::endl; }


    public:
      ~attribs          (void) { std::cout << " impl    attribs d'tor" << std::endl; }
      void attribs_test (void) { std::cout << " impl    attribs test"  << std::endl; }

      static saga::util::shared_ptr <impl::attribs> create (void)
      {
        return saga::util::shared_ptr <impl::attribs> (new impl::attribs ());
      }
  };


  ////////////////////////////////////////////////////////////////////////////////
  // in order to keep the pimpl hierarchy consistent, we continue to  
  // use 'virtual public' inheritance.  That is not strictly necessary 
  // (I think).
  class context : public virtual impl::object
                , public virtual impl::attribs 
  {
    protected:
      context           (void) { std::cout << " impl    context c'tor" << std::endl; }

    public:
      ~context          (void) { std::cout << " impl    context d'tor" << std::endl; } 
      void context_test (void) { std::cout << " impl    context test"  << std::endl; }

      static saga::util::shared_ptr <impl::context> create (void)
      {
        return saga::util::shared_ptr <impl::context> (new impl::context ());
      }
  };


} // namespace impl


////////////////////////////////////////////////////////////////////////////////
//
// facade classes
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
// This pimpl base class is not an actual part of the API, and instances cannot
// be created (protected c'tor).
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

    // make our class hierarchy polymorph.  This method can be removed - the
    // pimpl paradigm works just as well on non-polymorph facades.
    virtual void polymorph_ (void) 
    {
    }


  protected:
    // we don't want to expose the c'tors of this class on API level - the end
    // user should not need to explicitly create any pimpl instance.

    // the first c'tor is explicitly *not* getting an impl pointer: in the case 
    // of multiple inheritance, only one of the classes on any level of the 
    // class hierarchy will pass down the impl pointer - all others will invoke 
    // this c'tor for their base class.
    pimpl (noimpl_enum) 
    {
      std::cout << " facade  pimpl   c'tor (no impl)" << std::endl; 
      
      // if the facade hierarchy is spanned correctly, the following test
      // should work already!  Reason is that before the no_impl c'tor of this
      // base class is getting invoked, the derived class has already invoked 
      // the pimpl c'tor, and has thus already stored a valid impl pointer in
      // the pimpl base class.
      impl_test (); 
    }

    // the second c'tor is the one which gets passed down an impl pointer, which
    // is then stored for later usage.  Note that the storage type is the base
    // pointer of the impl hierarchy, but the passed pointer type is one of the
    // derived classes of the impl hierarchy.
    pimpl (saga::util::shared_ptr <impl::pimpl>  impl) 
      : impl_ (impl) 
    {
      std::cout << " facade  pimpl   c'tor (impl) - " 
                << impl_.get_ptype_demangled () << std::endl; 
      impl_test ();  // impl should be valid
    }


  public:
    // ~pimpl is where the reference counter for the shared pointers is
    // implicitly decremented, as the impl_ is getting out of scope.
    virtual ~pimpl (void)
    {
      std::cout << " facade  pimpl   d'tor" << std::endl; 
      
      impl_test ();
  
      if ( impl_ ) 
      {
        std::cout << " facade  pimpl d'tor - "   
                  << impl_.get_ptype_demangled () 
                  << " - " << impl_.get_count () << std::endl;
      }
    }

    // the get_impl() method is very central to our pimpl concept: it allows to
    // cast the impl pointer, which is stored in our virtual pimpl base class,
    // to be cast up and down the impl object hierarchy, so that the respective
    // impl method calls can be performed.
    //
    // Note that the dynamic_cast comes with a certain runtime penalty, as it
    // performs type checking at run time.  If that is starting to hurt your
    // program, you may consider to cache the returned and casted impl pointers
    // either here, or in your derived class.  The complexity will increase
    // however as one then needs to check for dangling pointers...
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
      std::cout << "COUNTME" << std::endl;

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


////////////////////////////////////////////////////////////////////////////////
//
// the object class is an intermediate class in our facade hierarchy, and as
// such passes the impl pointer down from any derived class to the pimpl base
// class.  But another c'tor allows to explicitly create an object instance
// - that c'tor then also creates its own impl pointer and passes it down.
//
class object : public virtual pimpl 
{
  protected:
    // the first c'tor does simply forward the noimpl flag.
    // this c'tor is only accessible by derived classes
    object (noimpl_enum noimpl) 
      : pimpl (noimpl)                
    { 
      std::cout << " facade  object  c'tor (no impl)" << std::endl; 
      impl_test ();
    }
    
    
  public:
    // the second (public) c'tor allows for explicit object creation, 
    // to, for example, satisfy STL container classes of saga::object's.
    object (void) 
      : pimpl (impl::object::create ().get_shared_ptr <impl::pimpl> ())    
    {
      std::cout << " facade  object  c'tor" << std::endl; 
      impl_test ();
    }
    
    ~object (void)
    {
      std::cout << " facade  object  d'tor" << std::endl; 
    }
    
    void object_test (void)  
    {
      std::cout << " object test" << std::endl; 
      impl_test ();
      std::cout << " object test" << std::endl; 
    }
};

////////////////////////////////////////////////////////////////////////////////
//
// attribs is an interface class, and thus does not create an impl pointer on
// construction.  In fact, it's c'tor is protected - only derived classes can 
// be created.
//
class attribs : public virtual pimpl 
{
  // interface class, thus protected c'tor/d'tor
  protected:
    // the c'tor does simply forward the noimpl flag.
    attribs (noimpl_enum noimpl) 
      : pimpl (noimpl)
    { 
      std::cout << " facade  attribs c'tor (no impl)" << std::endl; 
      impl_test ();
    }
    
    ~attribs (void)
    {
      std::cout << " facade  attribs d'tor" << std::endl; 
    }


  public:
    // the attribs methods
    void attribs_test (void)
    {
      std::cout << " attribs test" << std::endl; 
      impl_test ();
      std::cout << " attribs test" << std::endl; 
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// note that in the case of multiple inheritance, we need to make sure that
// none of the base classes is trying to store it's own impl pointer into the
// pimpl base class - thus the explicit call to the respectively safe c'tors.
//
class context : public object, public attribs 
{
  public:
    context (void) 
      : pimpl   (impl::context::create ().get_shared_ptr <impl::pimpl> ())
                                           // store new impl pointer in pimpl base
      , object  (pimpl::NO_IMPL)           // no impl pointer for the ...
      , attribs (pimpl::NO_IMPL)           // ... other base classes
    { 
      std::cout << " facade  context c'tor" << std::endl; 
      impl_test ();
    } 
    
    ~context (void) 
    {
      std::cout << " facade  context d'tor" << std::endl; 
    }
    
    void context_test (void) 
    {
      std::cout << " context test" << std::endl; 
      impl_test ();
      std::cout << " context test" << std::endl; 
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// main
//
int main (int argc, char** argv)
{ 
  try
  {
    for ( unsigned int i = 0; i < 10000; i++ )
    {
      //////////////////////////////////////////////////////////////////////////////
      // facade tests for object
      {
        std::cout << " 1 --------------------------------" << std::endl;

        object* p1 = new object ();
        pimpl*  p2 = p1;

        p1->object_test ();

        p1->impl_test ();
        p2->impl_test ();

        delete (p1);

        std::cout << " 2 --------------------------------" << std::endl;
      }
      //////////////////////////////////////////////////////////////////////////////
      
  
      //////////////////////////////////////////////////////////////////////////////
      // facade tests for context
      {
        std::cout << " 3 --------------------------------" << std::endl;

        context* p1 = new context ();
        object*  p2 = p1;
        attribs* p3 = p1;
        pimpl*   p4 = p1;

        p1->context_test ();

        p1->object_test  ();
        p2->object_test  ();

        p1->attribs_test ();
        p3->attribs_test ();

        p1->impl_test    ();
        p2->impl_test    ();
        p3->impl_test    ();
        p4->impl_test    ();

        delete (p1);

        std::cout << " 4 --------------------------------" << std::endl;
      }
      //////////////////////////////////////////////////////////////////////////////
    }
  } 
  catch ( const std::string & e )
  {
    std::cout << " exception: " << e << std::endl;
  }

  return 0;
} 

