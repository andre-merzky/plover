
#include <saga/engine/func.hpp>

namespace saga
{
  namespace impl
  {
    //////////////////////////////////////////////////////////////////
    func_base::func_base (std::string name, result_t * rp)
      : name_   (name)
      , result_ (rp)
    {
      SAGA_UTIL_STACKTRACE ();
    }

    func_base::~func_base (void) 
    {
      SAGA_UTIL_STACKTRACE ();
    }

    std::string func_base::get_name (void) 
    {
      return name_;  
    }

    void func_base::dump (std::string msg)
    {
      LOGSTR (DEBUG, "func_base dump")
        << " (" << this << ") : " 
        << saga::util::demangle (typeid (*this).name ()) 
        << std::endl;
    }

  } // namespace impl

} // namespace saga

