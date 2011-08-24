
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
      std::cout << "ctor  " << this << std::endl;
    }

    virtual ~test (void) 
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
    {
      saga::util::shared_ptr <test> t_0;
      // t_0->print ();
    }

    {
      saga::util::shared_ptr <test> t_1 (new test ());
      t_1->print ();

      {
        saga::util::shared_ptr <test> t_2;
        saga::util::shared_ptr <test> t_3 (t_1);
        {
          saga::util::shared_ptr <test> t_4 = t_3;
          t_2 = t_4;

          t_4->print (1);
        }
        t_3->print (2);

        saga::util::shared_ptr <test> t_5 = t_3.get_shared_ptr ();

        t_5->print (3);
        t_5->print (4);
      }
      t_1->print (5);
    }
  }
  catch (const std::exception & e ) 
  {
    std::cerr << "catched: " << e.what () << "\n";
  }
  catch (const char * & m ) 
  {
    std::cerr << "catched: " << m << "\n";
  }

  return 0;
}

