
#include <iostream>

#include "adaptor.hpp"

extern "C" 
{
  adaptor_1::object * get_object (void)
  {
    std::cout << "adaptor_1::get_object" << std::endl;

    adaptor_1::object * ret = new adaptor_1::object ();

    return ret;
  }
}

