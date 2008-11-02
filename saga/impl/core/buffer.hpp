
#ifndef SAGA_IMPL_BUFFER_HPP
#define SAGA_IMPL_BUFFER_HPP

#include <saga/impl/core/object.hpp>

// FIXME: this needs to be in impl
namespace saga
{
  namespace impl
  {
    class buffer : public object
    {
      private:
        char*   data_;
        ssize_t size_;
        bool    pwned_;
        bool    alloced_;

      protected:
        ssize_t read     (char*   source, 
                          size_t  length);

        ssize_t write    (char*   source, 
                          size_t  length);

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
}

#endif // SAGA_IMPL_BUFFER_HPP

