
#ifndef SAGA_IMPL_FILE_HPP
#define SAGA_IMPL_FILE_HPP

#include <saga/impl/core/object.hpp>

// FIXME: this needs to be in impl
namespace saga
{
  namespace impl
  {
    class file : public object
    {
      public:
        file          (void);
        ~file         (void);

        // from object interface
        saga::file  clone   (void) const;
    };
  }
}

#endif // SAGA_IMPL_FILE_HPP

