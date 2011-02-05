
#include <iostream>
#include <typeinfo>

//////////////////////////////////////////////////////////////////////
//
// test classes
//
class impl_base {
  public:
    virtual void polymorph (void) = 0;
    virtual void base_test ()
    {
      std::cout << "base test impl" << std::endl;
    }
};

class impl_object : public virtual impl_base {
  public:
    virtual void polymorph (void) { return; }
    virtual void id ()
    { 
      std::cout << "id" << std::endl;
    }
    virtual void object_test ()
    {
      std::cout << "object test impl" << std::endl;
    }
};

class impl_attribs : public virtual impl_base {
  public:
    virtual void polymorph (void) { return; }
    virtual void list ()
    {
      std::cout << "list" << std::endl;
    }
    virtual void attribs_test ()
    {
      std::cout << "attribs test impl" << std::endl;
    }
};

class impl_context : public impl_object, public impl_attribs {
  public:
    virtual void polymorph (void) { return; }
    virtual void context_test ()
    {
      std::cout << "context test impl" << std::endl;
    }
};

//////////////////////////////////////////////////////////////////////
//
// facade classes
//
class base {
  impl_base * impl_;
  public:
    base (void) { std::cout << "base c'tor" << std::endl; }
    base (impl_base * impl) : impl_ (impl) { 
      std::cout << "base c'tor (impl)" << std::endl; 
      base_test ();
    }

    impl_base * get_impl (void) { return impl_; }

    void base_test (void)
    {
      impl_base * impl = dynamic_cast <impl_base *> (get_impl ());
      impl->base_test ();
    }

    virtual void polymorph (void) { return; }
};

class object : public virtual base {
  public:
    object (void) { 
      std::cout << "object c'tor" << std::endl; 
      base_test ();
      object_test ();
    }
    virtual void id ()
    { 
      {
        impl_object * impl = dynamic_cast <impl_object *> (get_impl ());
        impl->id ();
      }
    }
    void object_test (void)
    {
      impl_object * impl = dynamic_cast <impl_object *> (get_impl ());
      impl->object_test ();
    }
};

class attribs : public virtual base {
  public:
    attribs (void) {
      std::cout << "attribs c'tor" << std::endl; 
      base_test ();
      attribs_test ();
    }
    virtual void list ()
    {
      impl_attribs * impl = dynamic_cast <impl_attribs *> (get_impl ());
      impl->list ();
    }
    void attribs_test (void)
    {
      impl_attribs * impl = dynamic_cast <impl_attribs *> (get_impl ());
      impl->attribs_test ();
    }
};

class context : public object, public attribs {
  public:
    context (void) : base (new impl_context ()) 
    { 
      std::cout << "context c'tor" << std::endl; 
      base_test ();
      object_test ();
      attribs_test ();
      context_test ();
    }
    void context_test (void)
    {
      impl_context * impl = dynamic_cast <impl_context *> (get_impl ());
      impl->context_test ();
    }
};


//////////////////////////////////////////////////////////////////////
//
// main
//
int main()
{
  {
                                           std::cout << "-----------------------------------" << std::endl;
    impl_context* p1 = new impl_context(); std::cout << "-----------------------------------" << std::endl;
    impl_object*  p2 = p1;                 std::cout << "-----------------------------------" << std::endl;
    impl_attribs* p3 = p1;                 std::cout << "-----------------------------------" << std::endl;
    impl_base*    p4 = p1;                 std::cout << "-----------------------------------" << std::endl;
                                           std::cout << "-----------------------------------" << std::endl;
    p1->id ();                             std::cout << "-----------------------------------" << std::endl;
    p2->id ();                             std::cout << "-----------------------------------" << std::endl;
                                           std::cout << "-----------------------------------" << std::endl;
    p1->list ();                           std::cout << "-----------------------------------" << std::endl;
    p3->list ();                           std::cout << "-----------------------------------" << std::endl;
                                           std::cout << "-----------------------------------" << std::endl;
    p4->base_test ();                      std::cout << "-----------------------------------" << std::endl;
  }

  {
                                           std::cout << "-----------------------------------" << std::endl;
    context* p1 = new context();           std::cout << "-----------------------------------" << std::endl;
    object*  p2 = p1;                      std::cout << "-----------------------------------" << std::endl;
    attribs* p3 = p1;                      std::cout << "-----------------------------------" << std::endl;
    base*    p4 = p1;                      std::cout << "-----------------------------------" << std::endl;
                                           std::cout << "-----------------------------------" << std::endl;
    p1->id();                              std::cout << "-----------------------------------" << std::endl;
    p2->id();                              std::cout << "-----------------------------------" << std::endl;
                                           std::cout << "-----------------------------------" << std::endl;
    p1->list();                            std::cout << "-----------------------------------" << std::endl;
    p3->list();                            std::cout << "-----------------------------------" << std::endl;
                                           std::cout << "-----------------------------------" << std::endl;
    p4->base_test ();                      std::cout << "-----------------------------------" << std::endl;
  }

  return 0;
} 

