
#include <saga/saga/core/session.hpp>
#include <saga/impl/core/session.hpp>

// c'tor
saga::session::session ()
  : saga::object (new saga::impl::session ())
{
}

// copy c'tor (shallow copy)
saga::session::session (saga::impl::session * impl)
  : saga::object (impl)
{
}

// copy c'tor (shallow copy)
saga::session::session (const session & src)
  : saga::object (src.get_obj_impl ())
{
}

// d'tor
saga::session::~session (void)
{
}

saga::impl::session * saga::session::none_    = NULL;
saga::impl::session * saga::session::default_ = NULL;

saga::session saga::session::no_session ()
{
 // return new saga session with the default impl
 return saga::session (none_);
}

saga::session saga::session::default_session ()
{
 // FIXME
 //
 // // if we get called the first time, create a default session
 // if ( NULL == default_ )
 // {
 //   default_ = new saga::impl::session ();
 // 
 //   // initialize default session
 //   // default_->get_default_contexts ();
 // }
 // 
 // // return new saga session with the default impl
 return saga::session (default_);
}

saga::util::shared_ptr <saga::impl::session> saga::session::get_obj_impl (void) const
{ 
  // from where should we get the impl?  (base class)
  typedef saga::object base_type;

  // type should the impl have?
  typedef saga::impl::session impl_type;

  // get impl from base class, and cast into correct type
  return boost::static_pointer_cast <impl_type> (this->base_type::get_obj_impl ());
}


saga::session::session (saga::util::shared_ptr <saga::impl::session> impl)
  : object (impl)
{
}

