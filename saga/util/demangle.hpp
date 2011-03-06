
#ifndef SAGA_UTIL_DEMANGLE_HPP
#define SAGA_UTIL_DEMANGLE_HPP

#include <string>

// FIXME: add configure test
#define HAVE_CXA_DEMANGLE 

namespace saga
{
  namespace util
  {
    std::string demangle (std::string name);
  }
}


#endif // SAGA_UTIL_DEMANGLE_HPP

