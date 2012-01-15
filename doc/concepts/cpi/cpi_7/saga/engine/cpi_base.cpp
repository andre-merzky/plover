
#include <saga/util/enums.hpp>

#include <saga/engine/cpi_base.hpp>


namespace saga
{
  namespace impl
  {
    SAGA_UTIL_REGISTER_ENUM (cpi_mode  , Any       , 0);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode  , Bounde    , 1);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode  , Simple    , 2);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode  , Collective, 3);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode  , Filter    , 4);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode  , All       , 5);

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

