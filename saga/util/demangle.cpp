
#include <saga/util/demangle.hpp>

// FIXME: add configure test
// #ifdef HAVE_CXA_DEMANGLE
#if 1
# include <cxxabi.h>
#endif

namespace saga
{
  namespace util
  {
    const char * demangle(const char* name)
    {

// #ifdef HAVE_CXA_DEMANGLE
#if 1
      // FIXME: code neither reentrant nor threadsafe
      size_t        size = 1024;
      static   char buf[1024];

      int status;

      char* res = abi::__cxa_demangle (name,
                                       buf,
                                       &size,
                                       &status);

      buf[sizeof (buf) - 1] = 0;

      return res;

#else

      return name;

#endif
    }

  } // namespace util

} // namespace saga
