
#include "result_types.hpp"

namespace saga
{
  namespace impl
  {
    std::ostream & operator<< (std::ostream & o, void_t const & v)
    {
      return o << "[void_t]";
    }


    //////////////////////////////////////////////////////////////////
    //
    // the result type thingies allow to keep get_result template free on impl
    // level and below - the template free base class is used wherever possible, 
    // and the types result_t_detail_ is only aquired as needed.
    //
    result_t::~result_t (void) 
    {
      SAGA_UTIL_STACKTRACE ();
    }

    void result_t::dump (std::string msg) 
    { 
      LOGSTR (DEBUG, "result_t, dump") << msg << " - (base result type)" << std::endl;
    }

  } // namespace impl

} // namespace saga

