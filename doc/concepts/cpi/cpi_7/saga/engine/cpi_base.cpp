
#include <saga/util/enums.hpp>

#include <saga/engine/cpi_base.hpp>


namespace saga
{
  namespace impl
  {
    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all CPIs
    //
    cpi_base::~cpi_base (void) {}

    void cpi_base::dump (std::string msg)
    {
      LOGSTR (DEBUG, "cpi_base dump")
        << "cpi_base (" << this << ") : " 
            << saga::util::demangle (typeid (*this).name ()) << " : " 
               << msg << std::endl;
    }

  } // namespace impl

} // namespace saga

