
#ifndef SAGA_DETAIL_ADAPTOR_HPP
#define SAGA_DETAIL_ADAPTOR_HPP

#include <string>

#include "adaptor_registry.hpp"

namespace saga
{
  namespace cpi
  {
    // forward declaration
    class object;
  }

  namespace detail
  {
    // FIXME: create_cpi should return a shared_ptr
    typedef cpi::object * (*create_hook_t)(void);
    typedef void          (*registration_hook_t)(saga::detail::adaptor_registry &);


    class adaptor
    {
      private:
        std::string libname_;
        void *      handle_;
        bool        loaded_;


      public:
         adaptor (std::string libname);
        ~adaptor (void);

        // // copy ctor, copies handle
        // adaptor (const adaptor & that)
        //   : handle_ (that.handle_)
        // {
        // }

        void load   (void);
        void unload (void);

        create_hook_t       get_create_hook       (void);
        registration_hook_t get_registration_hook (void);
    };

  } // namespace detail

} // namespace saga

#endif // SAGA_DETAIL_ADAPTOR_HPP


