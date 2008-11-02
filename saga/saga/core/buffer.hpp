
#ifndef SAGA_SAGA_BUFFER_HPP
#define SAGA_SAGA_BUFFER_HPP

#include <saga/util/shared_ptr.hpp>

#include <saga/saga/core/object.hpp>

namespace saga
{
  class buffer : public object
  {
    protected:
      // get impl from base class
      saga::util::shared_ptr <saga::impl::buffer> get_obj_impl (void) const;

    public:
      buffer           (ssize_t size = -1);
      buffer           (char*   data, 
                        ssize_t size = -1);

      ~buffer          (void);

      void    set_data (char*   data, 
                        size_t  size = -1);
      
      char*   get_data (void) const;

      void    set_size (ssize_t size = -1);

      ssize_t get_size (void) const;

      // from object interface
      buffer  clone (void);
  };
}

#endif // SAGA_SAGA_BUFFER_HPP

