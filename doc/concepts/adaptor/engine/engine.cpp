
#include <iostream>

#include "cpi.hpp"
#include "engine.hpp"
#include "adaptor.hpp"
#include "adaptor_registry.hpp"

namespace saga
{
  namespace detail
  {
    engine::engine (void)
    {
      std::cout << "engine c'tor" << std::endl;
    }


    engine::~engine (void)
    {
      std::cout << "engine d'tor" << std::endl;

      // all adaptors get unloaded on destruction
      for ( unsigned int i = 0; i < adaptors_.size (); i++ )
      {
        adaptors_[i].unload ();
      }
    }


    void engine::load_adaptor (std::string libname) 
    {
      std::cout << "engine load " << libname << std::endl;

      // load the adaptor in the given library path
      adaptor a (libname); 
      a.load ();


      // for the loaded adaptor, call the registration function
      // to register the cpi factories
      registration_hook_t register_cpi = a.get_registration_hook ();
      register_cpi (registry_);

      // keep adaptor for later unloading
      adaptors_.push_back (a);
    }



    void engine::run (void)
    {
      // all adaptor libs registered all adaptors for
      // specific cpis.  Now, get a list of all
      // saga::cpi::object adaptors, and test those cpi's
      std::vector <create_hook_t> create_hooks 
           = registry_.get_create_hooks <saga::cpi::object> ();

      for ( unsigned int i = 0; i < create_hooks.size (); i++ )
      {
        cpi::object * o = create_hooks[i]();

        o->init (1);

        delete (o);
      }
    }

  } // namespace detail

} // namespace saga

