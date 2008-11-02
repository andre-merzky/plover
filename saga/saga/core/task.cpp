
#include <saga/saga/core/task.hpp>
#include <saga/impl/core/task.hpp>

// c'tor
saga::task::task ()
  : saga::object      (saga::object::Task),
    saga::monitorable ()
{
}

// copy c'tor (shallow copy)
saga::task::task (saga::impl::task * impl)
  : saga::object      (impl),
    saga::monitorable (impl)
{
}

// copy c'tor (shallow copy)
saga::task::task (saga::util::shared_ptr <saga::impl::task> impl)
  : saga::object      (impl),
    saga::monitorable (impl)
{
}

// copy c'tor (shallow copy)
saga::task::task (const task & src)
  : saga::object      (src.get_obj_impl     ()),
    saga::monitorable (src.get_monitor_impl ())
{
}

// d'tor
saga::task::~task (void)
{
}

saga::util::shared_ptr <saga::impl::task> saga::task::get_obj_impl (void) const
{ 
  // from where should we get the impl?  (base class)
  typedef saga::object base_type;

  // type should the impl have?
  typedef saga::impl::task impl_type;

  // get impl from base class, and cast into correct type
  return boost::static_pointer_cast <impl_type> (this->base_type::get_obj_impl ());
}

saga::util::shared_ptr <saga::impl::task> saga::task::get_monitor_impl (void) const
{ 
  // from where should we get the impl?  (base class)
  typedef saga::monitorable base_type;

  // type should the impl have?
  typedef saga::impl::task impl_type;

  // get impl from base class, and cast into correct type
  return boost::static_pointer_cast <impl_type> (this->base_type::get_monitor_impl ());
}

