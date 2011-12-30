
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include "dir.hpp"

namespace saga
{
  namespace filesystem
  {
    dir::dir (std::string url)
      : impl_ (new saga::impl::filesystem::dir)
    {
      SAGA_UTIL_STACKTRACE ();
      (void) impl_->constructor (url);
    }

    std::string dir::get_url (void)
    {
      SAGA_UTIL_STACKTRACE ();
      return impl_->get_url ();
    }

    saga::filesystem::file dir::open (std::string url)
    {
      SAGA_UTIL_STACKTRACE ();
      return saga::filesystem::file (impl_->open (url));
    }

  } // namespace filesystem

} // namespace saga

