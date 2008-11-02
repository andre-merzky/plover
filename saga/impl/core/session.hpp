
#ifndef SAGA_IMPL_SESSION_HPP
#define SAGA_IMPL_SESSION_HPP

#include <saga/impl/core/object.hpp>

// FIXME: this needs to be in impl
namespace saga
{
  namespace impl
  {
    class session : public object
    {
      public:
        session          (void);
        ~session         (void);

        // from object interface
        saga::session  clone   (void) const;
    };
  }
}

#endif // SAGA_IMPL_SESSION_HPP

