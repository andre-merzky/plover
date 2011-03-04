
#ifndef SAGA_UTIL_DEMANGLE_HPP
#define SAGA_UTIL_DEMANGLE_HPP

#include <string>

// FIXME: add configure test
#define HAVE_CXA_DEMANGLE 

#ifdef  HAVE_CXA_DEMANGLE
# include <cxxabi.h>
#endif


namespace saga
{
  namespace util
  {
    std::string demangle (std::string name)
    {
#ifdef HAVE_CXA_DEMANGLE

      // FIXME: code neither reentrant nor threadsafe
      size_t        size = 1024;
      static   char buf[1024];
      int           status;

      char * res = abi::__cxa_demangle (name.c_str (), buf, &size, &status);

      buf[sizeof (buf) - 1] = 0;

      return (std::string (res));

#else

      return name;
#endif
    }
  }
}


#endif // SAGA_UTIL_DEMANGLE_HPP

