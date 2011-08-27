
#include <saga/util/demangle.hpp>

#include <string>    

// add configure test
#define HAVE_CXA_DEMANGLE 

#ifdef HAVE_CXA_DEMANGLE 
# include <cxxabi.h>
#endif

namespace saga
{
  namespace util
  {
    std::string demangle (std::string name)
    {
#ifndef HAVE_CXA_DEMANGLE      
      return name;    
#else          
      // FIXME: code neither reentrant nor threadsafe    
      size_t        size = 1024;
      static   char buf[1024];        
      int           status;        
      std::string   ret;
      
      char * res = abi::__cxa_demangle (name.c_str (), buf, &size, &status);          

      // FIXME: eval status and check return value
      
      buf[sizeof (buf) - 1] = 0;

      if ( res == NULL )
      {
        ret = name;
      }
      else
      {
        ret = res;
      }
      
      return (ret);

#endif // HAVE_CXA_DEMANGLE
    }

  } // namespace util

} // namespace saga

