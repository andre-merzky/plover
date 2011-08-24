
#include <saga/util/assert.hpp>

#include <iostream>    

namespace saga
{
  namespace util
  {
    void assert (bool check)
    {
#ifdef DEBUG
      if ( ! check )
      {
        std::cerr << "assert failed\n";
        ::abort ();
      }
#endif // DEBUG
    }

  } // namespace util

} // namespace saga

