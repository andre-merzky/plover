
#include <sstream>
#include <stdlib.h>

#include <saga/impl/core/object.hpp>

#include <saga/saga/core/exception.hpp>

namespace saga 
{
  namespace impl
  {
    object::object (saga::object::type type)
      : id_      (create_uuid ()),
        type_    (type)
    {
    }

    object::~object (void)
    {
    }

    std::string object::create_uuid (void)
    {
      std::ostringstream ss;

      ss << rand ();

      return ss.str ();
    }

    std::string object::get_id (void) const
    {
      return (id_);
    }

    void object::set_session (saga::session s)
    {
      session_ = s.get_obj_impl ();
    }

    saga::session object::get_session (void) const
    {
      if ( NULL == session_ )
      {
        throw saga::IncorrectState ("Object has no session");
      }

      return (saga::session (session_));
    }

    saga::object::type object::get_type (void) const
    {
      return (type_);
    }

    saga::object object::clone (void) const
    {
      // throw saga::NotImplemented 
      // ("clone() is not implemented on the base object");

      saga::object ret (type_);

      ret.get_obj_impl ()->set_session (saga::session (session_));

      return (ret);
    }

  } // ns impl
} // ns saga

