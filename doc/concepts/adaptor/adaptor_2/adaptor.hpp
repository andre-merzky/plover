
#ifndef ADAPTOR_HPP
#define ADAPTOR_HPP

#include "cpi.hpp"

namespace adaptor_2
{
  class object : public cpi::object
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

      void init (void)
      {
        std::cout << "adaptor_2::object::init" << std::endl;
      }
  };
}

extern "C" 
{
  adaptor_2::object * get_object (void);
}

#endif // ADAPTOR_HPP

