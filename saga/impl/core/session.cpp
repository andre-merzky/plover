
#include <saga/impl/core/session.hpp>
#include <saga/impl/core/object.hpp>

#include <saga/saga/core/session.hpp>

saga::impl::session::session (void)
  : object   (saga::object::Session)
{
}

saga::session saga::impl::session::clone (void) const
{
  saga::impl::session * tgt = new saga::impl::session ();
  
  // copy all contexts to new session

  return saga::session (tgt);
}


