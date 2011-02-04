
#include <saga/saga/core/buffer.hpp>
#include <saga/saga/core/exception.hpp>

#include <saga/impl/core/buffer.hpp>

saga::buffer::buffer (ssize_t size)
  : saga::object (new impl_type (size))
{
}

saga::buffer::buffer (char *  data, 
                      ssize_t size)
  : saga::object (new impl_type (data, size))
{
}

saga::buffer::buffer (char * const data )
  : saga::object (new saga::buffer::impl_type (data))
{
}

saga::buffer::~buffer (void)
{
}

void saga::buffer::set_data (char*  data, 
                             size_t size)
{
  get_impl <impl_type> ()->set_data (data, size);
}

char * saga::buffer::get_data (void) const
{
  return get_impl <impl_type> ()->get_data ();
}

void saga::buffer::set_size (ssize_t size)
{
  get_impl <impl_type> ()->set_size (size);
}


ssize_t saga::buffer::get_size (void) const
{
  return get_impl <impl_type> ()->get_size ();
}


template <class base_type>
saga::util::shared_ptr <saga::buffer::impl_type> saga::buffer::get_impl (void) const
{ 
  // get impl from base class, and cast into type of this's implementation
  saga::util::shared_ptr <base_type> bp = this->base_type::get_obj_impl <base_type> ();
  saga::util::shared_ptr <impl_type> ip =        bp.static_pointer_cast <impl_type> ();

  return ip;
}

