
#ifndef SAGA_IMPL_TASK_HPP
#define SAGA_IMPL_TASK_HPP

#include <saga/impl/core/object.hpp>
#include <saga/impl/core/monitorable.hpp>

// FIXME: this needs to be in impl
namespace saga
{
  namespace impl
  {
    class task : public object, 
                 public monitorable
    {
      public:
        task  (void);
        ~task (void);
    };
  }
}

#endif // SAGA_IMPL_TASK_HPP

