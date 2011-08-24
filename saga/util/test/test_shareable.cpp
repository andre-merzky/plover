
#include <iostream>

#include <saga/util/shareable.hpp>
#include <saga/util/shared_ptr.hpp>

class test : public saga::util::shareable
{
  private:
    int i_;


  public:
    test (void) 
      : i_ (0)
    { 
      std::cout << "ctor          : " << this << std::endl;
    }

    ~test (void) 
    { 
      std::cout << "dtor          : " << this << std::endl;
    }

    void check (void)
    {
      std::cout << "test      : " << i_ << " : " << this << std::endl;
      saga::util::shared_ptr <test> sp = shared_this <test> ();
      std::cout << "TEst      : " << i_ << " : " << this << std::endl;
      sp->print ();
    }

    void print (int i = 0)
    { 
      i_++;
      std::cout << "print : " << i << " : " << i_ << " : " << this << std::endl;
    }
};

int main ()
{
  try
  {
    saga::util::shared_ptr <test> sp1 (new test);
    sp1->print ();
    sp1->check ();

    std::cout << " 1 ----------------------" << std::endl;
    test * p1 = new test;
    saga::util::shared_ptr <test> sp4 (p1);
    {
      std::cout << " 2 a ----------------------" << std::endl;
      saga::util::shared_ptr <test> sp2 = p1->shared_this <test> ();
      std::cout << " 2 b----------------------" << std::endl;
    }
    std::cout << " 3 ----------------------" << std::endl;

    saga::util::shared_ptr <test> sp3 = p1->shared_this <test> ();
    std::cout << " 4 ----------------------" << std::endl;

    p1->print ();
    p1->check ();
    p1->check ();

    saga::util::shared_ptr <test> sp2 = p1->shared_this <test> ();
    sp2->print ();
    sp2->check ();
  }
  catch (const std::exception & e ) 
  {
    std::cerr << "catched: " << e.what () << "\n";
  }
  catch (const char * e ) 
  {
    std::cerr << "catched: " << e << "\n";
  }

  return 0;
}

