
////////////////////////////////////////////////////////////////////////////////
//
// PIMPL for Polymorph Class Hierarchies 
// -------------------------------------
//
// PIMPL: 'Pointer to IMPLementation'
//        see also http://en.wikipedia.org/wiki/Opaque_pointer
// 
// The PIMPL paradigm suggests to devide a class implementation into two
// separate parts: a light-weight facade (or interface) class, and a full blown
// implementation class.  The facade is what the end user is dealing with.  It
// contains a single private data member: a pointer to the implementation class
// (or, short, and impl pointer).  All method invocations on the facade are
// forwarded to that impl pointer, like
//
//   int facade::sum (int a, int b)
//   {
//     return impl->sum (a, b);
//   }
//
// That way, the facade's interface can be kept very stable (also binary 
// stable), and the impl side is well encapsulated, and can change if needed,
// w/o screwing with the end user code at all.
//
// This document demonstrated how the same paradigm can be applied to a complete
// class hierarchy, which can also include multiple inheritance.  Both the
// facade and the impl hierarchy show the same inheritance structure.  A single
// impl pointer is maintained in the facade's base class, and is used by all
// derived facade classes for call forwarding.  For that to work it requires the
// ability to dynamic_cast that impl pointer up and down the impl class
// hierarchy (via a templatized get_impl() method), which in turn implies that 
// the impl base class needs to be polymorph.  In our example, the facade
// hierarchy is also polymorph, although that is actually no requirement at all
// (we don't cast facade pointers in our example).
//
// Note that the presented code is not threadsafe, and in general has a number 
// of consistency and memory management problems.  Those can be mostly addressed
// by using a shared_ptr for the impl pointer, as shown in
// doc_pimpl_shared_pointers.cpp.
//

#include <iostream>  // for debug messages
#include <typeinfo>  // for better debug messages
#include <string>    // we use strings as exceptions *blush*

#include <saga/util/demangle.hpp>
#include <saga/util/shareable.hpp>

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
  // Note that in this exmaple code, polymorphism is used on both sides of 
  // the hierarchy.  That is, depending on the application code, not really
  // required for the pimpl scheme to work.
  //
  class pimpl : public saga::util::shareable
  {
    private:
      // the impl::pimpl class needs to be polymorphic for the up/down casting of
      // its (and its decendent's) pointers to work (via dynamic_cast).  So, we add
      // one virtual function, but hide it.
      virtual void polymorph_ (void) 
      {
      }

    public:
      pimpl (void) { std::cout << " impl    pimpl   c'tor" << std::endl; }

      // we need a virtual d'tor, too.  Note that all d'tors in derived classes
      // will automatically be virtual now - no need to declare that again
      virtual ~pimpl (void) 
      {
        std::cout << " impl    pimpl   d'tor" << std::endl; 
      }

      // other methods do not need to be virtual (but can be if needed).  In
      // general, impl::pimpl should be kept empty, to make its job (to act as
      // polymorphic impl base) more obvious.
      void impl_test (void) 
      {
        std::cout << " impl    pimpl   test" << std::endl; 
      }
  };


  ////////////////////////////////////////////////////////////////////////////////
  // other impl classes *must*, directly or indirectly, inherit impl::pimpl as
  // 'virtual public base class', to profit from its polymorphism properties.
  class object  : public virtual impl::pimpl 
  {
    public:
      object           (void) { std::cout << " impl    object  c'tor" << std::endl; }
      ~object          (void) { std::cout << " impl    object  d'tor" << std::endl; }
      void object_test (void) { std::cout << " impl    object  test"  << std::endl; }
  };


  ////////////////////////////////////////////////////////////////////////////////
  class attribs_base : public virtual impl::pimpl 
  {
    public:
      attribs_base      (void) { std::cout << " impl    attribs_base c'tor" << std::endl; }
      ~attribs_base     (void) { std::cout << " impl    attribs_base d'tor" << std::endl; }
      void attribs_test (void) { std::cout << " impl    attribs_base test"  << std::endl; }
  };


  ////////////////////////////////////////////////////////////////////////////////
  class attribs : public virtual impl::attribs_base
  {
    public:
      attribs           (void) { std::cout << " impl    attribs c'tor" << std::endl; }
      ~attribs          (void) { std::cout << " impl    attribs d'tor" << std::endl; }
      void attribs_test (void) { std::cout << " impl    attribs test"  << std::endl; }
  };


  ////////////////////////////////////////////////////////////////////////////////
  // in order to keep the pimpl hierarchy consistent, we continue to  
  // use 'virtual public' inheriatance.  That is not strictly neccessary 
  // (I think).
  class context : public virtual impl::object
                , public virtual impl::attribs 
  {
    public:
      context           (void) { std::cout << " impl    context c'tor" << std::endl; }
      ~context          (void) { std::cout << " impl    context d'tor" << std::endl; } 
      void context_test (void) { std::cout << " impl    context test"  << std::endl; }
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
// class in the facade's inheritance tree, the pointer is retreived again from 
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
    enum _noimpl_enum
    {
      NO_IMPL = 1
    };


  private:
    // here we keep the pointer to the impl instance, cast' to it's base type
    impl::pimpl * impl_;

    // make our class hierarchy polymorph.  This method can be removed - the
    // pimpl paradigms works just as well on non-polymorph facades.
    virtual void polymorph_ (void) 
    {
    }


  protected:
    // we don't want to expose the c'tors of this class on API level - the end
    // user should not need to explicitely create any pimpl instance.

    // the first c'tor is explicitely *not* getting an impl pointer: in the case 
    // of multiple inheritance, only one of the classes on any level of the 
    // class hierarchy will pass down the impl pointer - all others will invoke 
    // this c'tor for their base class.
    pimpl (_noimpl_enum) 
      : impl_ (NULL) // ?? FIXME
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
    pimpl (impl::pimpl * impl) 
      : impl_ (impl) 
    {
      std::cout << " facade  pimpl   c'tor (impl) - " 
                << saga::util::demangle (typeid (*impl_).name ()) << std::endl; 

      impl_test ();  // impl should be valid
    }


  public:
    // ~pimpl should be the only place where the impl pointer gets deleted
    virtual ~pimpl (void)
    {
      std::cout << " facade  pimpl   d'tor" << std::endl; 
      
      impl_test ();
  
      if ( NULL != impl_ ) 
      {
        std::cout << " facade  pimpl d'tor - " 
                  << saga::util::demangle (typeid (*impl_).name ()) <<  std::endl;
        delete (impl_);
        impl_ = NULL;
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
    // obviously...
    template <typename T>
    T *  get_impl (void)              
    { 
      // ensure we actually have an impl pointer.  This avoids an exception on
      // the cast later on.
      if ( NULL == impl_ )
      {
        throw std::string (" pimpl::get_impl<T>() on NULL impl pointer!");
      }

      // try to cast to the wanted pointer type
      T * ret = dynamic_cast <T *> (impl_); 

      // if that fails, complain
      if ( NULL == ret )
      {
        throw std::string (" pimpl::get_impl <")
          + saga::util::demangle (typeid (T).name ())
          + "> () cannot cast an " 
          + saga::util::demangle (typeid (*impl_).name ())
          + " impl pointer!";
      }

      // otherwise return the thus casted pointer.
      // Note that we hand out a second pointer to the same impl instance,
      // with the obvious potential for problems for consistency (double delete, 
      // access after delete, etc)
      return (ret);
    }


    // is_a() is very similar to get_impl: it simply checks if get_impl, and in
    // particular the casting, would succeed.
    template <typename T>
    bool is_a (void)              
    { 
      // return false on a NULL pointer (which has no type)
      // the cast later on.
      if ( NULL == impl_ )
      {
        return false;
      }

      // try to cast to the wanted pointer type
      T * ret = dynamic_cast <T *> (impl_); 

      // if that fails, complain
      if ( NULL == ret )
      {
        // cast failed, so impl is not a T.
        return false;
      }

      // otherwise, impl is a T
      return true;
    }


    // just a test method, which also demonstrated the use of is_a<T>() and 
    // get_impl<T>().
    void impl_test (void) 
    {
      std::cout << " pimpl test" << std::endl; 
      if ( is_a <impl::pimpl>   () )   get_impl <impl::pimpl>   ()->impl_test    ();
      if ( is_a <impl::object>  () )   get_impl <impl::object>  ()->object_test  ();
      if ( is_a <impl::attribs> () )   get_impl <impl::attribs> ()->attribs_test ();
      if ( is_a <impl::context> () )   get_impl <impl::context> ()->context_test ();
      std::cout << " pimpl test" << std::endl; 
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// the object class is an intermediate class in out facade hierarchy, and as
// such passed the impl pointer down from any derived class to the pimpl base
// class.  But another c'tor allows to explicitely create an object instance
// - that c'tor then also creates its own impl pointer and passes it on.
//
class object : public virtual pimpl 
{
  protected:
    // the first c'tor does simply forward the noimpl flag.
    object (_noimpl_enum) 
      : pimpl (pimpl::NO_IMPL)                
    { 
      std::cout << " facade  object  c'tor (no impl)" << std::endl; 
      impl_test ();
    }
    
    
  public:
    // the second (public) c'tor allows for explicit object creation.
    object (void) 
      : pimpl (new impl::object ())    
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
class attribs_base : public virtual pimpl 
{
  // interface class, thus protected c'tor/d'tor
  protected:
    // the c'tor does simply forward the noimpl flag.
    attribs_base (_noimpl_enum) 
      : pimpl (pimpl::NO_IMPL)                
    { 
      std::cout << " facade  attribs_base c'tor (no impl)" << std::endl; 
      impl_test ();
    }
    
    ~attribs_base (void)
    {
      std::cout << " facade  attribs_base d'tor" << std::endl; 
    }


  public:
    // the attribs_base methods
    void attribs_base_test (void)
    {
      std::cout << " attribs_base test" << std::endl; 
      impl_test ();
      std::cout << " attribs_base test" << std::endl; 
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// attribs is an interface class, and thus does not create an impl pointer on
// construction.  In fact, it's c'tor is protected - only derived classes can 
// be created.
//
class attribs : public attribs_base
{
  // interface class, thus protected c'tor/d'tor
  protected:
    // the c'tor does simply forward the noimpl flag.
    attribs (_noimpl_enum ni) 
      : attribs_base (ni)                
      , pimpl        (ni)                
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
      : pimpl   (new impl::context ()) // store new impl pointer in virt. pimpl base
      , object  (pimpl::NO_IMPL)       // no impl pointer for the ...
      , attribs (pimpl::NO_IMPL)       // ... other base classes
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
    std::cout << " 1 --------------------------------" << std::endl;

    //////////////////////////////////////////////////////////////////////////////
    // impl tests
    {
      std::cout << " 2 --------------------------------" << std::endl;

      impl::context* p1 = new impl::context ();
      impl::object*  p2 = p1;
      impl::attribs* p3 = p1;
      impl::pimpl*   p4 = p1;

      p1->context_test ();

      p1->object_test ();
      p2->object_test ();

      p1->attribs_test ();
      p3->attribs_test ();

      p1->impl_test ();
      p2->impl_test ();
      p3->impl_test ();
      p4->impl_test ();

      delete (p1);

      std::cout << " 4 --------------------------------" << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////////
    // facade tests for object
    {
      std::cout << " 5 --------------------------------" << std::endl;

      object* p1 = new object ();
      pimpl*  p2 = p1;

      p1->object_test ();

      p1->impl_test ();
      p2->impl_test ();

      delete (p1);

      std::cout << " 6 --------------------------------" << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////////
    
  
    //////////////////////////////////////////////////////////////////////////////
    // facade tests for context
    {
      std::cout << " 7 --------------------------------" << std::endl;

      context* p1 = new context ();
      object*  p2 = p1;
      attribs* p3 = p1;
      pimpl*   p4 = p1;

      p1->context_test ();

      p1->object_test ();
      p2->object_test ();

      p1->attribs_test ();
      p3->attribs_test ();

      p1->impl_test ();
      p2->impl_test ();
      p3->impl_test ();
      p4->impl_test ();

      delete (p1);

      std::cout << " 8 --------------------------------" << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////////
  } 
  catch ( const std::string & e )
  {
    std::cout << " exception: " << e << std::endl;
  }

  return 0;
} 

