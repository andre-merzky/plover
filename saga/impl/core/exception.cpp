
#include <saga/impl/core/exception.hpp>

saga::impl::exception::exception (std::string msg)
  : msg_ (msg)
{
}

saga::impl::exception::~exception (void)
{
}

std::string saga::impl::exception::get_msg (void) const
{
  return (msg_);
}

