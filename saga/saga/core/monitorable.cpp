

#include <saga/saga/core/monitorable.hpp>
#include <saga/impl/core/monitorable.hpp>

saga::monitorable::monitorable  (void)
  : monitor_impl_ (new saga::impl::monitorable ())
{
}

// copy c'tor (shallow copy)
saga::monitorable::monitorable (saga::impl::monitorable * impl)
  : monitor_impl_ (impl)
{
}

// copy c'tor (shallow copy)
saga::monitorable::monitorable (saga::util::shared_ptr <impl::monitorable> impl)
  : monitor_impl_ (impl) 
{
}

// copy c'tor (shallow copy)
saga::monitorable::monitorable (const monitorable & src)
  : monitor_impl_ (src.get_monitor_impl ())
{
}

saga::monitorable::~monitorable (void)
{
}

saga::metric saga::monitorable::get_metric (std::string name)
{                 
  return monitor_impl_->get_metric (name);
}                 

std::list <std::string> saga::monitorable::list_metrics (void)
{
  return monitor_impl_->list_metrics ();
}

int saga::monitorable::add_callback (std::string name, 
                                     callback    cb)
{
  return monitor_impl_->add_callback (name, cb);
}

void saga::monitorable::remove_callback (int cookie)
{
  monitor_impl_->remove_callback (cookie);
}

saga::util::shared_ptr <saga::impl::monitorable> saga::monitorable::get_monitor_impl (void) const
{ 
  return monitor_impl_;
}

