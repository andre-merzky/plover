
#include <iostream>

#include <saga/util/shared_ptr.hpp>

class test 
{
  private:
    int * i_;

    void inc_ (void)
    {
      *i_ += + 1;
    }

  public:
    test (void) 
    { 
      i_ = new int;
    }
    ~test (void) 
    { 
      delete i_;
    }

    void print (void)
    { 
      inc_ ();
    }
};

int main ()
{
  try
  {
    saga::util::shared_ptr <test> t_1 (new test ());
    {
      saga::util::shared_ptr <test> t_0;
      saga::util::shared_ptr <test> t_2 (t_1);
      {
        saga::util::shared_ptr <test> t_3 = t_2;
        t_0 = t_3;

        t_3->print ();
      }
      t_2->print ();
    }
    t_1->print ();
  }
  catch (const std::exception & e ) 
  {
    std::cerr << "catched: " << e.what () << "\n";
  }

  return 0;
}

