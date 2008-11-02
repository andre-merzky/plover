
#include <set>
#include <iostream>

#include <boost/assign/list_of.hpp>

#include <saga/saga/saga.hpp>

int main ()
{
  try 
  {
    saga::session s;

    saga::context c1;
    saga::context c0;

    c1.set_attribute ("testkey", "testval 1");
    c0.set_attribute ("testkey", "testval 2");

    saga::context c2 (c1);
    saga::context c3 = c1;
    saga::context c4 = c0; c4 = c1;


    std::cout << "id  0 : " << c0.get_id () << std::endl;
    std::cout << "id  1 : " << c1.get_id () << std::endl;
    std::cout << "id  2 : " << c2.get_id () << std::endl;
    std::cout << "id  3 : " << c3.get_id () << std::endl;
    std::cout << "id  4 : " << c4.get_id () << std::endl;

    std::cout << "val 0 : " << c0.get_attribute ("testkey") << std::endl;
    std::cout << "val 1 : " << c1.get_attribute ("testkey") << std::endl;
    std::cout << "val 2 : " << c2.get_attribute ("testkey") << std::endl;
    std::cout << "val 3 : " << c3.get_attribute ("testkey") << std::endl;
    std::cout << "val 4 : " << c4.get_attribute ("testkey") << std::endl;

    saga::file f;
  }
  catch ( const saga::exception::base & e )
  {
    std::cout << "Error: " << e.get_msg () << std::endl;
  }

  return 0;
}

