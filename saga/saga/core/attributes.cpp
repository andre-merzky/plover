
#include <saga/saga/core/attributes.hpp>
#include <saga/impl/core/attributes.hpp>

saga::attributes::attributes  (std::set <std::string> ro_s, 
                               std::set <std::string> ro_v, 
                               std::set <std::string> rw_s, 
                               std::set <std::string> rw_v, 
                               bool             extensible)
  : attr_impl_ (new impl_type (ro_s, ro_v, rw_s, rw_v, extensible))
{
}

// copy c'tor (shallow copy)
saga::attributes::attributes (impl_type * impl)
: attr_impl_ (impl)
{
}

// copy c'tor (shallow copy)
saga::attributes::attributes (saga::util::shared_ptr <impl::attributes> impl)
  : attr_impl_ (impl) 
{
}

// copy c'tor (shallow copy)
saga::attributes::attributes (const attributes & src)
  : attr_impl_ (src.get_attr_impl <saga::attributes::impl_type> ())
{
}

saga::attributes::~attributes (void)
{
}

std::string saga::attributes::get_attribute (std::string key)
{                 
  return attr_impl_->get_attribute (key);
}                 

void saga::attributes::set_attribute (std::string key, 
                                      std::string val)
{
  attr_impl_->set_attribute (key, val);
}

std::set <std::string> saga::attributes::get_vector_attribute (std::string key)
{
  return attr_impl_->get_vector_attribute (key);
}

void saga::attributes::set_vector_attribute (std::string key, 
                                             std::set <std::string> val)
{
  attr_impl_->set_vector_attribute (key, val);
}


bool saga::attributes::attribute_exists (std::string key)
{
  return attr_impl_->attribute_exists (key);
}

bool saga::attributes::attribute_is_readonly (std::string key)
{
  return attr_impl_->attribute_is_readonly (key);
}

bool saga::attributes::attribute_is_writable (std::string key)
{
  return attr_impl_->attribute_is_writable (key);
}

bool saga::attributes::attribute_is_scalar (std::string key)
{
  return attr_impl_->attribute_is_scalar (key);
}

bool saga::attributes::attribute_is_vector (std::string key)
{
  return attr_impl_->attribute_is_vector (key);
}

template <class T>
saga::util::shared_ptr <saga::attributes::impl_type> saga::attributes::get_attr_impl (void) const
{ 
  return attr_impl_;
}

template <class T>
saga::util::shared_ptr <saga::attributes::impl_type> saga::attributes::get_impl (void) const
{ 
  return get_attr_impl <saga::attributes::impl_type> ();
}
