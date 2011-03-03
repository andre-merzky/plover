
#include <iostream>

#include "adaptor.hpp"

extern "C" 
{
  saga::cpi::object * create_cpi (void)
  {
    std::cout << "adaptor_2::create_cpi" << std::endl;

    adaptor_2::object * ret = new adaptor_2::object ();

    return ret;
  }

  void register_cpi (saga::detail::adaptor_registry & reg)
  {
    saga::detail::create_hook_t hook = create_cpi;
    reg.register_cpi <saga::cpi::object> (hook);
  }
}

