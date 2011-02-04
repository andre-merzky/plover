
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
  : saga::object      (src.get_impl <saga::object>      ()),
    saga::monitorable (src.get_impl <saga::monitorable> ())
{
}

// d'tor
saga::task::~task (void)
{
}

template <class base_type>
saga::util::shared_ptr <impl_type> saga::task::get_impl (void) const
{ 
  // get impl from base class, and cast into type of this's implementation
  saga::util::shared_ptr <base_type> bp = this->base_type::get_impl <base_type> ();
  saga::util::shared_ptr <impl_type> ip = bp.static_pointer_cast    <impl_type> ();
  
  return ip;
}

