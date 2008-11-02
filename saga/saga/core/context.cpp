
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
saga::context::context (saga::impl::context * impl)
  : saga::object     (impl),
    saga::attributes (impl)
{
}

// copy c'tor (shallow copy)
saga::context::context (saga::util::shared_ptr <saga::impl::context> impl)
  : saga::object     (impl),
    saga::attributes (impl)
{
}

// copy c'tor (shallow copy)
saga::context::context (const context & src)
  : saga::object     (src.get_obj_impl  ()),
    saga::attributes (src.get_attr_impl ())
{
}

// d'tor
saga::context::~context (void)
{
}

saga::util::shared_ptr <saga::impl::context> saga::context::get_obj_impl (void) const
{ 
  // from where should we get the impl?  (base class)
  typedef saga::object base_type;

  // type should the impl have?
  typedef saga::impl::context impl_type;

  // get impl from base class, and cast into correct type
  return boost::static_pointer_cast <impl_type> (this->base_type::get_obj_impl ());
}

saga::util::shared_ptr <saga::impl::context> saga::context::get_attr_impl (void) const
{ 
  // from where should we get the impl?  (base class)
  typedef saga::attributes base_type;

  // type should the impl have?
  typedef saga::impl::context impl_type;

  // get impl from base class, and cast into correct type
  return boost::static_pointer_cast <impl_type> (this->base_type::get_attr_impl ());
}

