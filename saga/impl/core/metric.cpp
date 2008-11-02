
#include <set>
#include <string>

#include <saga/impl/core/metric.hpp>
#include <saga/impl/core/object.hpp>

#include <saga/saga/core/metric.hpp>

saga::impl::metric::metric  (std::set <std::string> attrib_ro_s, 
                               std::set <std::string> attrib_ro_v, 
                               std::set <std::string> attrib_rw_s, 
                               std::set <std::string> attrib_rw_v, 
                               bool                   extensible)
  : object     (saga::object::Metric),
    attributes (attrib_ro_s, 
                attrib_ro_v, 
                attrib_rw_s, 
                attrib_rw_v, 
                extensible)
{
}

// saga::metric saga::impl::metric::clone (void) const
// {
//   saga::impl::metric * tgt = new saga::impl::metric ();
//   
//   // copy all metric to new metric
// 
//   return saga::metric (tgt);
// }


