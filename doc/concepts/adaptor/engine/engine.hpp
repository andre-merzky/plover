
#ifndef SAGA_DETAIL_ENGINE_HPP
#define SAGA_DETAIL_ENGINE_HPP

#include <vector>
#include <string>

#include "adaptor_registry.hpp"

namespace saga
{
  namespace detail
  {
    // forward declarations
    class adaptor;

    class engine
    {
      private:
        // vector of laded adaptors
        // FIXME: should be shared_ptr <adaptor>
        std::vector <adaptor> adaptors_; 

        // the adaptor_registry maps the cpi type to functions
        // which create instances of that cpi, from various
        // adaptors.
        saga::detail::adaptor_registry registry_;


      public:
        engine (void);
        ~engine (void);

        void load_adaptor   (std::string libname);
        void run            (void);
    };

  } // namespace detail

} // namespace saga



#endif // SAGA_DETAIL_ENGINE_HPP

