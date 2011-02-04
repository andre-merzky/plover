
#include <set>
#include <string>

#include <saga/util/assign.hpp>

#include <saga/saga/core/context.hpp>
#include <saga/impl/core/context.hpp>

static std::set <std::string> attrib_ro_s = saga::util::assign_set <std::string> ("s1")("s2");
static std::set <std::string> attrib_ro_v = saga::util::assign_set <std::string> ("v1")("v2");
static std::set <std::string> attrib_rw_s = saga::util::assign_set <std::string> ("S1")("S2");
static std::set <std::string> attrib_rw_v = saga::util::assign_set <std::string> ("V1")("V2");

// c'tor
saga::context::context ()
  : saga::object     (saga::object::Context),
    saga::attributes (attrib_ro_s, 
                      attrib_ro_v, 
                      attrib_rw_s, 
                      attrib_rw_v, 
                      true)
{
}

// copy c'tor (shallow copy)
saga::context::context (impl_type * impl)
  : saga::object     (impl),
    saga::attributes (impl)
{
}

// copy c'tor (shallow copy)
saga::context::context (saga::util::shared_ptr <impl_type> impl)
  : saga::object     (impl),
    saga::attributes (impl)
{
}

// copy c'tor (shallow copy)
saga::context::context (const context & src)
  : saga::object     (src.get_impl <saga::impl::object>     ()),
    saga::attributes (src.get_impl <saga::impl::attributes> ())
{
}

// d'tor
saga::context::~context (void)
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

