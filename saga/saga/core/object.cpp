
#include <saga/saga/core/object.hpp>
#include <saga/saga/core/session.hpp>

#include <saga/impl/core/object.hpp>
#include <saga/impl/core/session.hpp>

// c'tor
saga::object::object (saga::object::type t)
  : obj_impl_    (new saga::impl::object (t)),
    has_session_ (false)
{
}

// c'tor
saga::object::object (saga::object::type t, 
                      saga::session      s)
  : obj_impl_     (new saga::impl::object (t)),
    has_session_  (true),
    session_impl_ (s.get_obj_impl ()) // FIXME is that right?  not get_impl?
{
}

// copy c'tor (shallow copy)
saga::object::object (saga::impl::object * impl)
  : obj_impl_ (impl)
{
}

// copy c'tor (shallow copy)
saga::object::object (saga::util::shared_ptr <impl::object> impl)
  : obj_impl_ (impl) 
{
}

// copy c'tor (shallow copy)
saga::object::object (const object & src)
  : obj_impl_ (src.get_obj_impl ())
{
}

// d'tor
//
// The session which this oibject was attached to is getting released.
saga::object::~object (void)
{
}

// get_id
std::string saga::object::get_id (void) const
{
  return obj_impl_->get_id ();      
}

// get_type
saga::object::type saga::object::get_type (void) const
{ 
  return obj_impl_->get_type ();    
}

// get_session
saga::session saga::object::get_session (void) const
{
  return saga::session (session_impl_); 
}

// deep copy
saga::object saga::object::clone (void) const
{
  return saga::object (obj_impl_->clone ());
}


// get object impl pointer, for friends only!
saga::util::shared_ptr <saga::impl::object> saga::object::get_obj_impl (void) const
{ 
  return obj_impl_;
}

