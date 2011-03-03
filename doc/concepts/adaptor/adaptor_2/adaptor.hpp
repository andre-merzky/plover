
#ifndef ADAPTOR_HPP
#define ADAPTOR_HPP

#include "../engine/cpi.hpp"
#include "../engine/adaptor_registry.hpp"

namespace adaptor_2
{
  class object : public saga::cpi::object
  {
    public:
      object (void)
      {
        std::cout << "adaptor_2::object::c'tor" << std::endl;
      }

      ~object (void)
      {
        std::cout << "adaptor_2::object::d'tor" << std::endl;
      }

      void init (int i)
      {
        std::cout << "adaptor_2::object::init: " << i << std::endl;
      }
  };
}

extern "C" 
{
  saga::cpi::object * create_cpi   (void);
  void                register_cpi (saga::detail::adaptor_registry & reg);
}

#endif // ADAPTOR_HPP

