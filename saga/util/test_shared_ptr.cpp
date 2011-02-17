
#include <iostream>

#include <saga/util/shared_ptr.hpp>

class test 
{
  private:
    int i_;


  public:
    test (void) 
      : i_ (0)
    { 
      std::cout << "ctor  " << this << std::endl;
    }

    ~test (void) 
    { 
      std::cout << "dtor  " << this << std::endl;
    }

    void print (int i = 0)
    { 
      i_++;
      std::cout << "print " << i << " : " << i_ << " : " << this << std::endl;
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

        t_3->print (1);
      }
      t_2->print (2);

      saga::util::shared_ptr <test> t_4 = t_2.get_shared_ptr ();
      t_4->print (3);
      t_4->print (4);
    }
    t_1->print (5);
  }
  catch (const std::exception & e ) 
  {
    std::cerr << "catched: " << e.what () << "\n";
  }

  return 0;
}

