
#include <iostream>

#include <saga/util/lockable.hpp>
#include <saga/util/scoped_lock.hpp>

class test : public saga::util::lockable
{
};

int main ()
{
  try
  {
    test t;

    {
      saga::util::scoped_lock sl (t);
    }

    t.lock     ();
    t.try_lock ();
    t.unlock   ();
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

