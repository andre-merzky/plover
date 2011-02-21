
#include <iostream>
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "cpi.hpp"

//////////////////////////////////////////////////////////////////////
//
// Yet another central element in SAGA is the notion of adaptors.  
//
//


// Writing: func = (int (*)(void)) dlsym(handle, "adaptor_test");
// would seem more natural, but the C99 standard leaves
// casting from "void *" to a function pointer undefined.
// The assignment used below is the POSIX.1-2003 (Technical
// Corrigendum 1) workaround; see the Rationale for the
// POSIX specification of dlsym(). */

int main ()
{
  std::vector <std::string> adaptor_list;

  adaptor_list.push_back ("adaptor_1/libadaptor_1.so");
  adaptor_list.push_back ("adaptor_2/libadaptor_2.so");

  for ( unsigned int i = 0; i < adaptor_list.size (); i++ )
  {
    std::cout << "------------------------" << std::endl;

    void * handle = ::dlopen (adaptor_list[i].c_str (), RTLD_LAZY);

    if ( ! handle )
    {
      std::cout << ::dlerror () << std::endl;
      return (-1);
    }


    ::dlerror ();    /* Clear any existing error */


    cpi::object * (*func)(void);
    *(void **) (&func) = ::dlsym (handle, "get_object");

    char* error = ::dlerror ();

    if ( NULL != error )
    {
      std::cout << error << std::endl;
      return (-1);
    }

    cpi::object * o = (*func)();

    o->init ();

    delete (o);

    ::dlclose (handle);

    std::cout << "------------------------" << std::endl;
  }
  
  return (0);
}

