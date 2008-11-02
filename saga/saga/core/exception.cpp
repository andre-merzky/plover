
#include <saga/saga/core/exception.hpp>
#include <saga/impl/core/exception.hpp>

saga::exception::exception (std::string msg)
  : impl_ (new saga::impl::exception (msg))
{
}

std::string saga::exception::get_msg (void) const
{
  return impl_->get_msg ();
}

// FIXME: complete...


