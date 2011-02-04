
#include <saga/saga/core/session.hpp>
#include <saga/impl/core/session.hpp>

// static session instances
impl_type * saga::session::none_    = NULL;
impl_type * saga::session::default_ = NULL;


// c'tor
saga::session::session ()
  : saga::object (new impl_type ())
{
}

// c'tor from impl
saga::session::session (impl_type * impl)
  : saga::object (impl)
{
}

saga::session::session (saga::util::shared_ptr <impl_type> impl)
  : saga::object (impl)
{
}


// copy c'tor (shallow copy)
saga::session::session (const session & src)
  : saga::object (src.get_impl <saga::object> ())
{
}

// d'tor
saga::session::~session (void)
{
}

saga::session saga::session::no_session ()
{
  // return new saga session with the default impl
  // no contexts are attached
  return saga::session (none_);
}

saga::session saga::session::default_session ()
{
  // FIXME
  //
  // // if we get called the first time, create a default session
  // if ( NULL == default_ )
  // {
  //   default_ = new impl_type ();
  // 
  //   // initialize default session
  //   // default_->get_default_contexts ();
  // }
  // 
  // // return new saga session with the default impl
  return saga::session (default_);
}


template <class base_type>
saga::util::shared_ptr <impl_type> saga::task::get_impl (void) const
{ 
  // get impl from base class, and cast into type of this's implementation
  saga::util::shared_ptr <base_type> bp = this->base_type::get_impl <base_type> ();
  saga::util::shared_ptr <impl_type> ip = bp.static_pointer_cast    <impl_type> ();

  return ip;
}

