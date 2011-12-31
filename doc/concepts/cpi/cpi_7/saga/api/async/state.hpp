
#ifndef SAGA_API_ASYNC_STATE_HPP
#define SAGA_API_ASYNC_STATE_HPP

#include <saga/util/enums.hpp>

namespace saga
{
  namespace async
  {
    enum mode
    { 
      Sync  = 0,
      Async = 1,
      Task  = 2
    };

    SAGA_UTIL_REGISTER_ENUM (mode, Sync , 0);
    SAGA_UTIL_REGISTER_ENUM (mode, Async, 1);
    SAGA_UTIL_REGISTER_ENUM (mode, Task , 2);


    enum state 
    {
      Unknown    = 0,
      New        = 1,
      Running    = 2,
      Done       = 3,
      Failed     = 4,
      Canceled   = 5
    };
  
    SAGA_UTIL_REGISTER_ENUM (state, Unknown   , 0);
    SAGA_UTIL_REGISTER_ENUM (state, New       , 1);
    SAGA_UTIL_REGISTER_ENUM (state, Running   , 2);
    SAGA_UTIL_REGISTER_ENUM (state, Done      , 3);
    SAGA_UTIL_REGISTER_ENUM (state, Failed    , 4);
    SAGA_UTIL_REGISTER_ENUM (state, Canceled  , 5);

  } // namespace async

} // namespace saga

#endif //  SAGA_API_ASYNC_STATE_HPP

