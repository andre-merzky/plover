
#include <iostream>

#include "adaptor.hpp"

extern "C" 
{
  saga::util::shared_ptr <saga::cpi::object> create_object_cpi (void)
  {
    std::cout << "adaptor_2::create_cpi" << std::endl;

    return saga::util::shared_ptr <adaptor_2::object> (new adaptor_2::object ());
  }

  void register_cpi (saga::detail::adaptor_registry & reg)
  {
    reg.register_cpi_object <saga::cpi::object> (create_object_cpi);
  }
}

