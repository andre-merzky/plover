
#include <set>
#include <string>

#include <saga/util/assign.hpp>

#include <saga/saga/core/metric.hpp>
#include <saga/impl/core/metric.hpp>

static std::set <std::string> attrib_ro_s = saga::util::assign::list_of ("s1")("s2");
static std::set <std::string> attrib_ro_v = saga::util::assign::list_of ("v1")("v2");
static std::set <std::string> attrib_rw_s = saga::util::assign::list_of ("S1")("S2");
static std::set <std::string> attrib_rw_v = saga::util::assign::list_of ("V1")("V2");

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
saga::metric::metric (impl_type * impl)
  : saga::object     (impl),
    saga::attributes (impl)
{
}

// copy c'tor (shallow copy)
saga::metric::metric (saga::util::shared_ptr <impl_type> impl)
  : saga::object     (impl),
    saga::attributes (impl)
{
}

// copy c'tor (shallow copy)
saga::metric::metric (const metric & src)
  : saga::object     (src.get_impl <saga_object>      ()),
    saga::attributes (src.get_impl <saga::attributes> ())
{
}

// d'tor
saga::metric::~metric (void)
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

