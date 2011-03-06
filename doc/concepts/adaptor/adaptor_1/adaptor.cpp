
#include <iostream>

#include "adaptor.hpp"

extern "C" 
{
  // factory method to create instances of this adaptor's implementation of the
  // object cpi
  saga::util::shared_ptr <saga::cpi::object> create_object_cpi (void)
  {
    std::cout << "adaptor_1::create_object_cpi" << std::endl;

    return saga::util::shared_ptr <adaptor_1::object> (new adaptor_1::object ());
  }


  // factory method to create instances of this adaptor's implementation of the
  // context cpi
  saga::util::shared_ptr <saga::cpi::object> create_context_cpi (void)
  {
    std::cout << "adaptor_1::create_context_cpi" << std::endl;

    return saga::util::shared_ptr <adaptor_1::context> (new adaptor_1::context ());
  }


  // this method with well defined name is unique to the adaptor library, and is
  // called just after the lib got dynamically loaded.  The method adds the two 
  // factory methods to the registry map in the saga engine,   
  void register_cpi (saga::detail::adaptor_registry & reg)
  {
    reg.register_cpi_object <saga::cpi::object>  (create_object_cpi);
    reg.register_cpi_object <saga::cpi::context> (create_context_cpi);
  }
}

