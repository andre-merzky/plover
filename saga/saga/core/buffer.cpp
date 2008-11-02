
#include <saga/saga/core/buffer.hpp>
#include <saga/saga/core/exception.hpp>

#include <saga/impl/core/buffer.hpp>

saga::buffer::buffer (ssize_t size)
  : saga::object (new saga::impl::buffer (size))
{
}

saga::buffer::buffer (char*   data, 
                      ssize_t size)
  : saga::object (new saga::impl::buffer (data, size))
{
}

saga::buffer::~buffer (void)
{
}

void saga::buffer::set_data (char*  data, 
                             size_t size)
{
  get_obj_impl ()->set_data (data, size);
}

char * saga::buffer::get_data (void) const
{
  return get_obj_impl ()->get_data ();
}

void saga::buffer::set_size (ssize_t size)
{
  get_obj_impl ()->set_size (size);
}


ssize_t saga::buffer::get_size (void) const
{
  return get_obj_impl ()->get_size ();
}

saga::util::shared_ptr <saga::impl::buffer> saga::buffer::get_obj_impl (void) const
{ 
  // from where should we get the impl?  (base class)
  typedef saga::object base_type;

  // type should the impl have?
  typedef saga::impl::buffer impl_type;

  // get impl from base class, and cast into correct type
  return boost::static_pointer_cast <impl_type> (this->base_type::get_obj_impl ());
}

