
#include <list>

#include <saga/impl/core/monitorable.hpp>

#include <saga/saga/core/metric.hpp>
#include <saga/saga/core/callback.hpp>
#include <saga/saga/core/exception.hpp>

// FIXME: get initial metrics, and bool extensible
saga::impl::monitorable::monitorable  (void)
{
}

saga::impl::monitorable::~monitorable (void)
{
}

saga::metric saga::impl::monitorable::get_metric (std::string name)
{
  metric_exists (name, true);

  return (metrics_[name]);
}

std::list <std::string> saga::impl::monitorable::list_metrics (void)
{
  std::list <std::string> out;
  std::map  <std::string, saga::metric> :: iterator i;

  for ( i  = metrics_.begin (); 
        i != metrics_.end   (); ++i )
  {
    out.push_back (i->first);
  }

  return (out);
}

int saga::impl::monitorable::add_callback (std::string name, 
                                           saga::callback cb)
{
  metric_exists (name, true);

  int cookie = cb_count_;
  cb_count_++;

  std::pair <std::string, saga::callback> p (name, cb);

  callbacks_[cookie] = p;

  return (cookie);
}

void saga::impl::monitorable::remove_callback (int cookie)
{
  if ( callbacks_.find (cookie) == callbacks_.end () )
  {
    throw saga::DoesNotExist ("no such callback");
  }

  callbacks_.erase (cookie);
}

bool saga::impl::monitorable::metric_exists (std::string name, 
                                             bool        except)
{
  std::map <std::string, saga::metric> :: iterator i;

  if ( metrics_.find (name) != metrics_.end () )
  {
    return true;
  }

  if ( except )
  {
    throw saga::DoesNotExist (name + " : no such metric");
  }

  return (false);
}

