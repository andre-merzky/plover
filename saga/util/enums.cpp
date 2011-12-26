
#include <saga/util/enums.hpp>
#include <saga/util/singleton.hpp>

namespace saga
{
  namespace util
  {
    void enums_dump (void)
    {
      saga_enums::get_singleton ()->dump ();
    }

  } // namespace util

} // namespace saga


