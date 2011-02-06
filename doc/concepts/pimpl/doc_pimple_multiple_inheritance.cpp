
#include <iostream>
#include <typeinfo>
#include <string>

//////////////////////////////////////////////////////////////////////
class my_exception : public std::exception
{
  private:
    std::string msg_;

  public:
    my_exception (const std::string & msg) : msg_ (msg) {}
    ~my_exception() throw () {};

    const char* msg (void) const { return msg_.c_str (); }
};



//////////////////////////////////////////////////////////////////////
//
// impl classes
//

//////////////////////////////////////////////////////////////////////
// pimpl_impl is a virtually (pun!) empty class, which serves as polymorphism
// anchor for all impl classes.  That way we can reflect the polymorph
// inheritance hierarchy of the facade side, and ensure that dynamic_cast's up
// and down that hierarchy function as expected.
//
// Note that polymorphism is only used on the impl side of the hierarchy - the
// facade end is not intented for up-and-down casting that way.
class pimpl_impl 
{
  public:
    // the pimpl_impl class needs to be polymorphic for the up/down casting of
    // its, and its decendents pointers to work (via dynamic_cast).
    virtual void polymorph    (void) = 0;

    // other methods do not need to be virtual (but can be if needed)
    void base_test    (void) { std::cout << " pimpl   test impl" << std::endl; }
};

//////////////////////////////////////////////////////////////////////
// other impl classes *must*, directly or indirectly, inherit pimpl_impl as
// 'virtual public base class', to profit from its polymorphism properties.
class impl_object  : public virtual pimpl_impl 
{
  public:
    void polymorph    (void) { return; }
    void object_test  (void) { std::cout << " object  test impl" << std::endl; }
};

//////////////////////////////////////////////////////////////////////
class impl_attribs : public virtual pimpl_impl 
{
  public:
    void polymorph    (void) { return; }
    void attribs_test (void) { std::cout << " attribs test impl" << std::endl; }
};

//////////////////////////////////////////////////////////////////////
// in order to keep the pimpl hierarchy intact beyond the API's own scope, it is
// adevisable to consistently use 'virtual public' inheriatance.
class impl_context : public virtual impl_object
                   , public virtual impl_attribs 
{
  public:
    void polymorph    (void) { return; }
    void context_test (void) { std::cout << " context test impl" << std::endl; }
};



//////////////////////////////////////////////////////////////////////
//
// facade classes
//
//
// The facade end of the pimpl mechanism needs to maintain a pointer to the impl
// classes somewhere.  That is done by this pimpl base class.  That very impl
// pointer is then used by all derived classes to invoke the actual methods on
// that impl pointer.  The impl pointer must thus be of the type of the most
// derived class (i.e. must be a context_impl* for a context facade class).
// That class passes the impl pointer created on construction time to the pimpl
// base class for storage.
//
// Once the impl pointer is needed by the facade class, or by any other class in
// the facade's inheritance tree, the pointer is retreived again from the pimpl
// base class, and dynamic_cast'ed back into its appropriate type.
//
// This class is not an actual part of the API!
// 
class pimpl 
{
  public:
    // enum to tag c'tors without impl pointer
    enum pimpl_enum
    {
      NO_IMPL = 0
    };

  private:
    // here we keep the pointer to the impl instance, cast to it's base type
    pimpl_impl * impl_;

  public:
    pimpl (void)              : impl_ (NULL)      { std::cout << " pimpl   c'tor" << std::endl; }
    pimpl (pimpl_enum)                            { std::cout << " pimpl   c'tor (no impl)" << std::endl; 
                                                    base_test (); }
    pimpl (pimpl_impl * impl) : impl_ (impl)      { std::cout << " pimpl   c'tor (impl)" << std::endl; 
                                                    base_test (); }

    // pimpl should be the only place where the impl pointer gets deleted
    ~pimpl (void) {
      std::cout << " pimpl   d'tor" << std::endl;
      if ( NULL != impl_ ) {
        std::cout << " pimpl   deletes impl_" << std::endl;
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
    // however as one then needs to check for dangling pointers...
    template <typename T>
    T *  get_impl (void)              
    { 
      // ensure we actually have an impl pointer.  This avoids an exception on
      // the cast later on.
      if ( NULL == impl_ )
      {
        throw my_exception ("pimpl::get_impl() on NULL impl pointer!");
      }

      // std::cout << " pimpl::get_impl " << impl_ << " - " << typeid(*impl_).name () << std::endl;  

      // try to cast to the wanted pointer type
      T * ret = dynamic_cast <T *> (impl_); 

      // if that fails, complain
      if ( NULL == ret )
      {
        throw my_exception (std::string ("pimpl::get_impl <")
                            + typeid (*ret).name ()
                            + "> () cannot cast an " 
                            + typeid (*impl_).name () 
                            + " impl pointer!");
      }

      // otherwise return the thus casted pointer.
      // Note that we hand out a second pointer to the same impl instance,
      // with the obvious potential for problems for consistency (double delete, 
      // access after delete, etc)
      return (ret);
    }

    void  base_test (void) { get_impl <pimpl_impl> ()->base_test (); }
};

//////////////////////////////////////////////////////////////////////
class object : public virtual pimpl 
{
  public:
    object (void) : pimpl (new impl_object ())    { std::cout << " object  c'tor" << std::endl; 
                                                    base_test   ();
                                                    object_test (); }
    object (pimpl_enum pe) : pimpl (pe)           { std::cout << " object  c'tor (no impl)" << std::endl; 
                                                    base_test   ();
                                                    object_test (); }
    object (impl_object * impl) : pimpl (impl)    { std::cout << " object  c'tor (impl)" << std::endl; 
                                                    base_test   ();
                                                    object_test (); }
    ~object                   (void)              { std::cout << " object  d'tor" << std::endl; }
    void object_test          (void)              { get_impl <impl_object> ()->object_test (); }
};

//////////////////////////////////////////////////////////////////////
class attribs : public virtual pimpl 
{
  public:
    attribs                   (void)              { std::cout << " attribs c'tor" << std::endl; 
                                                    base_test    ();
                                                    attribs_test (); } 
    ~attribs                  (void)              { std::cout << " attribs d'tor" << std::endl; }
    void attribs_test         (void)              { get_impl <impl_attribs> ()->attribs_test (); }
};

//////////////////////////////////////////////////////////////////////
class context : public object, public attribs 
{
  public:
    context (void) : pimpl  (new impl_context ())
                   , object (pimpl::NO_IMPL)      { std::cout << " context c'tor" << std::endl; 
                                                    base_test    ();
                                                    object_test  ();
                                                    attribs_test ();
                                                    context_test (); } 
    ~context                  (void)              { std::cout << " context d'tor" << std::endl; }
    void context_test         (void)              { get_impl <impl_context> ()->context_test (); }
};


//////////////////////////////////////////////////////////////////////
//
// main
//
int main (int argc, char** argv)
{ 
  try
  {
    std::cout << " 1 --------------------------------" << std::endl;

    ////////////////////////////////////////////////////////////////////
    // impl tests
    {
      std::cout << " 2 --------------------------------" << std::endl;

      impl_context* p1 = new impl_context ();

      std::cout << " 3 --------------------------------" << std::endl;

      impl_object*  p2 = p1;
      impl_attribs* p3 = p1;
      pimpl_impl*    p4 = p1;

      p1->context_test ();

      p1->object_test ();
      p2->object_test ();

      p1->attribs_test ();
      p3->attribs_test ();

      p1->base_test ();
      p2->base_test ();
      p3->base_test ();
      p4->base_test ();

      delete (p1);

      std::cout << " 4 --------------------------------" << std::endl;
    }
    ////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////
    // facade tests for object
    {
      std::cout << " 5 --------------------------------" << std::endl;

      object* p1 = new object ();

      std::cout << " 5 1 ------------------------------" << std::endl;

      pimpl*   p2 = p1;

      p1->object_test ();

      p1->base_test ();
      p2->base_test ();

      delete (p1);

      std::cout << " 6 --------------------------------" << std::endl;
    }
    ////////////////////////////////////////////////////////////////////
    
  
    ////////////////////////////////////////////////////////////////////
    // facade tests for context
    {
      std::cout << " 7 --------------------------------" << std::endl;

      context* p1 = new context ();

      std::cout << " 7 1 ------------------------------" << std::endl;

      object*  p2 = p1;
      attribs* p3 = p1;
      pimpl*   p4 = p1;

      p1->context_test ();

      p1->object_test ();
      p2->object_test ();

      p1->attribs_test ();
      p3->attribs_test ();

      p1->base_test ();
      p2->base_test ();
      p3->base_test ();
      p4->base_test ();

      delete (p1);

      std::cout << " 8 --------------------------------" << std::endl;
    }
    ////////////////////////////////////////////////////////////////////
  } 
  catch ( const my_exception & e )
  {
    std::cout << " exception: " << e.msg () << std::endl;
  }

  return 0;
} 

