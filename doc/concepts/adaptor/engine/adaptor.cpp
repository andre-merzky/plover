
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "adaptor.hpp"


namespace saga
{
  namespace detail
  {
    adaptor::adaptor (std::string libname)
      : libname_ (libname)
      , handle_  (NULL)
      , loaded_  (false)
    {
    }


    adaptor::~adaptor (void)
    {
    }


    void adaptor::load (void)
    {

      ::dlerror (); // Clear any existing error

      handle_ = (char*) ::dlopen (libname_.c_str (), RTLD_LAZY);

      if ( ! handle_ )
      {
        char * err = ::dlerror ();
        std::cout << err << std::endl;
        throw (err);
      }

      loaded_ = true;

      std::cout << "adaptor load ok: " << handle_ << std::endl;
    }

    
    create_hook_t adaptor::get_create_hook (void)
    {
      if ( ! loaded_ )
      {
        throw "adaptor not leaded";
      }

      // a small note on the dlsym function handle assignment:
      //
      //   create_cpi = (int (*)(void)) dlsym(handle, "create_cpi");
      //
      // would seem to be the right way to obtain a function
      // handle from dlsym, but the C99 standard leaves casting
      // from "void *" to a function pointer undefined.  The
      // assignment we use instead is
      //
      //   *(void **) (&create_cpi) = ::dlsym (handle, "create_cpi");
      //
      // This is the POSIX.1-2003 (Technical Corrigendum 1)
      // workaround; see the Rationale for the POSIX specification
      // of dlsym().
      //
      // NOTE: cast is at the moment done in engine::run().
      //
      // create_hook_t ret;
      // *(void **) (&ret) = ::dlsym (handle_, "create_cpi");
      create_hook_t ret = (create_hook_t) ::dlsym (handle_, "create_cpi");
      
      char * err = ::dlerror ();

      if ( err )
      {
        std::cout << err << std::endl;
        throw err;
      }

      return ret;
    }


    registration_hook_t adaptor::get_registration_hook (void)
    {
      if ( ! loaded_ )
      {
        throw "adaptor not leaded";
      }

      registration_hook_t ret = (registration_hook_t) ::dlsym (handle_, "register_cpi");
      
      char * err = ::dlerror ();

      if ( err )
      {
        std::cout << err << std::endl;
        throw err;
      }

      return ret;
    }


    void adaptor::unload (void)
    {
      std::cout << "adaptor unload " << handle_ << std::endl;

      if ( ! loaded_ )
      {
        std::cout << "double unload? " << handle_ << std::endl;
        return;
      }

      if ( handle_ )
      {
        ::dlerror (); // Clear any existing error
        ::dlclose (handle_);
        handle_ = NULL;
      }

      char * err = ::dlerror ();

      if ( err )
      {
        std::cout << err << std::endl;
      }

      return;
    }


  } // namespace detail

} // namespace saga


