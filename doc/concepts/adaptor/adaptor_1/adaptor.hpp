
#ifndef ADAPTOR_HPP
#define ADAPTOR_HPP

#include "cpi.hpp"

namespace adaptor_1
{
  class object : public cpi::object
  {
    public:
      object (void)
      {
        std::cout << "adaptor_1::object::c'tor" << std::endl;
      }

      ~object (void)
      {
        std::cout << "adaptor_1::object::d'tor" << std::endl;
      }

      void init (void)
      {
        std::cout << "adaptor_1::object::init" << std::endl;
      }
  };
}

extern "C" 
{
  adaptor_1::object * get_object (void);
}

#endif // ADAPTOR_HPP

