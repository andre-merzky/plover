
#include <set>
#include <string>

#include <saga/saga/core/metric.hpp>
#include <saga/impl/core/metric.hpp>

static std::set <std::string> attrib_ro_s = boost::assign::list_of ("s1")("s2");
static std::set <std::string> attrib_ro_v = boost::assign::list_of ("v1")("v2");
static std::set <std::string> attrib_rw_s = boost::assign::list_of ("S1")("S2");
static std::set <std::string> attrib_rw_v = boost::assign::list_of ("V1")("V2");

// c'tor
saga::metric::metric ()
  : saga::object     (saga::object::Metric),
    saga::attributes (attrib_ro_s, 
                      attrib_ro_v, 
                      attrib_rw_s, 
                      attrib_rw_v, 
                      true)
{
}

// copy c'tor (shallow copy)
saga::metric::metric (saga::impl::metric * impl)
  : saga::object     (impl),
    saga::attributes (impl)
{
}

// copy c'tor (shallow copy)
saga::metric::metric (saga::util::shared_ptr <saga::impl::metric> impl)
  : saga::object     (impl),
    saga::attributes (impl)
{
}

// copy c'tor (shallow copy)
saga::metric::metric (const metric & src)
  : saga::object     (src.get_obj_impl  ()),
    saga::attributes (src.get_attr_impl ())
{
}

// d'tor
saga::metric::~metric (void)
{
}

saga::util::shared_ptr <saga::impl::metric> saga::metric::get_obj_impl (void) const
{ 
  // from where should we get the impl?  (base class)
  typedef saga::object base_type;

  // type should the impl have?
  typedef saga::impl::metric impl_type;

  // get impl from base class, and cast into correct type
  return boost::static_pointer_cast <impl_type> (this->base_type::get_obj_impl ());
}

saga::util::shared_ptr <saga::impl::metric> saga::metric::get_attr_impl (void) const
{ 
  // from where should we get the impl?  (base class)
  typedef saga::attributes base_type;

  // type should the impl have?
  typedef saga::impl::metric impl_type;

  // get impl from base class, and cast into correct type
  return boost::static_pointer_cast <impl_type> (this->base_type::get_attr_impl ());
}

