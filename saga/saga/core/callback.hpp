
#ifndef SAGA_SAGA_CALLBACK_HPP
#define SAGA_SAGA_CALLBACK_HPP

#include <saga/util/shared_ptr.hpp>

#include <saga/saga/core/object.hpp>
#include <saga/saga/core/attributes.hpp>

namespace saga
{
  class callback
  {
    public:
               callback (void);
      virtual ~callback (void);

      // the real callback method
      // virtual bool cb (monitorable & mon, 
      //                  metric      & met, 
      //                  context     & ctx);
  };
}


#endif // SAGA_SAGA_CALLBACK_HPP

