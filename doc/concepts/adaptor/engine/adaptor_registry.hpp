
#ifndef   SAGA_ADAPTOR_REGISTRATOR_HPP
#define   SAGA_ADAPTOR_REGISTRATOR_HPP

#include <map>
#include <vector>
#include <iostream>
#include <typeinfo>

#include <saga/util/demangle.hpp>
#include <saga/util/shareable.hpp>
#include <saga/util/shared_ptr.hpp>

#include "cpi.hpp"


namespace saga
{
  namespace detail
  {
    class adaptor_registry;

    // FIXME: create_cpi should return a shared_ptr
    typedef saga::util::shared_ptr <cpi::object> (*create_hook_t)(void);
    typedef void          (*registration_hook_t)(saga::detail::adaptor_registry &);


    class adaptor_registry
    {
      private:
        std::map <std::string, std::vector <create_hook_t> > create_hook_map_;

      public:
        template <typename T>
        void register_cpi_object (create_hook_t hook)
        {
          std::cout << "register_cpi_object: " 
                    << saga::util::demangle (typeid (T).name ())
                    << std::endl;
          create_hook_map_ [saga::util::demangle (typeid (T).name ())].push_back (hook);
        }


        template <class T>
        std::vector <create_hook_t> get_create_hooks (void)
        {
          return create_hook_map_ [saga::util::demangle (typeid (T).name ())];
        }
    };

  } // namespace saga

} // namespace detail

#endif // SAGA_ADAPTOR_REGISTRATOR_HPP

