
#ifndef SAGA_SAGA_BUFFER_HPP
#define SAGA_SAGA_BUFFER_HPP

#include <saga/util/shared_ptr.hpp>

#include <saga/saga/core/object.hpp>

namespace saga
{
  namespace impl
  {
    class buffer;
  }

  class buffer : public object
  {
    typedef saga::impl::buffer impl_type;

    friend class saga::impl::buffer;


    protected:
      // create context from impl
      buffer (                        impl_type * impl);
      buffer (saga::util::shared_ptr <impl_type>  impl);

      // get impl from base classes
      template <class base_type>
      saga::util::shared_ptr <impl_type> get_impl (void) const;



    public:
      buffer           (ssize_t      size = -1);
      buffer           (char *       data, 
                        ssize_t      size = -1);
      buffer           (char * const data );

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

