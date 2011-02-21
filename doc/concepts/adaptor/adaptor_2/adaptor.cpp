
#include <iostream>

#include "adaptor.hpp"

extern "C" 
{
  adaptor_2::object * get_object (void)
  {
    std::cout << "adaptor_2::get_object" << std::endl;

    adaptor_2::object * ret = new adaptor_2::object ();

    return ret;
  }
}

