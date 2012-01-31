
#include "result_types.hpp"

namespace saga
{
  namespace impl
  {
    std::ostream & operator<< (std::ostream & o, void_t const & v)
    {
      return o << "[void_t]";
    }

  } // namespace impl

} // namespace saga

