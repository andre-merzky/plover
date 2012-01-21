
#include <saga/engine/functors.hpp>

namespace saga
{
  namespace impl
  {
    //////////////////////////////////////////////////////////////////
    functor_base::functor_base (std::string name)
      : name_ (name)
    {
      SAGA_UTIL_STACKTRACE ();
    }

    functor_base::~functor_base (void) 
    {
      SAGA_UTIL_STACKTRACE ();
    }

    std::string functor_base::get_name (void) 
    {
      return name_;  
    }

    void functor_base::dump (std::string msg)
    {
      LOGSTR (DEBUG, "functor_base dump")
        << " (" << this << ") : " 
        << saga::util::demangle (typeid (*this).name ()) 
        << std::endl;
    }

  } // namespace impl

} // namespace saga

