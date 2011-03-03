
#include <iostream>

#include "adaptor.hpp"

extern "C" 
{
  saga::cpi::object * create_object_cpi (void)
  {
    std::cout << "adaptor_1::create_object_cpi" << std::endl;

    adaptor_1::object * ret = new adaptor_1::object ();

    return ret;
  }


  saga::cpi::object * create_context_cpi (void)
  {
    std::cout << "adaptor_1::create_context_cpi" << std::endl;

    adaptor_1::context * ret = new adaptor_1::context ();

    return ret;
  }

  void register_cpi (saga::detail::adaptor_registry & reg)
  {
    reg.register_cpi <saga::cpi::object>  (create_object_cpi);
    reg.register_cpi <saga::cpi::context> (create_context_cpi);
  }
}

