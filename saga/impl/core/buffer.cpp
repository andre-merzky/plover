
#include <saga/impl/core/buffer.hpp>
#include <saga/impl/core/object.hpp>

#include <saga/saga/core/buffer.hpp>
#include <saga/saga/core/exception.hpp>

// FIXME: NoSession!
saga::impl::buffer::buffer (ssize_t size)
  : object   (saga::object::Buffer),
    size_    (size),
    pwned_   (true),
    alloced_ (false)
{
  // alloc data if required
  set_size (size_);
}

saga::impl::buffer::buffer (char*   data, 
                            ssize_t size)
  : object   (saga::object::Buffer),
    data_    (data),
    size_    (size),
    pwned_   (false),
    alloced_ (false)
{
  // set_data (data, size);
  // not needed, as its done by the setters
}

ssize_t saga::impl::buffer::read (char*  source, 
                                  size_t length)
{
  if ( pwned_  && ! alloced_ )
  {
    // alloc on the fly if required
    set_size (length);
  }

  if ( pwned_  && size_t (size_) < length )
  {
    // re-alloc on the fly if required
    set_size (length);
  }

  // check apps memory size
  if ( size_t (size_) < length )
  {
    throw saga::BadParameter ("oops!");
  }

  // all set - get data
  memcpy (data_, source, length);

  return length;
}


ssize_t saga::impl::buffer::write (char*  target, 
                                   size_t length)
{
  // can't re-alloc on the fly (memory would be empty and
  // useless...)

  // check apps or impl memory size
  if ( size_t (size_) < length )
  {
    throw saga::BadParameter ("oops!");
  }

  // all set - write data
  memcpy (target, data_, length);

  return length;
}


void saga::impl::buffer::set_data (char*  data, 
                                  size_t size)
{
  // delete old data if owned by impl
  if ( pwned_ && alloced_ )
  {
    delete (data_);
    alloced_ = false;
  }
  
  // keep apps data pointer
  data_  = data;
  size_  = size;
  pwned_ = false;
}

char * saga::impl::buffer::get_data (void) const
{
  // give data pointer to apps
  return data_; 
}

void saga::impl::buffer::set_size (ssize_t size)
{
  size_ = size;

  // delete old data if owned by impl
  if ( pwned_ && alloced_ )
  {
    delete (data_);
    alloced_ = false;
  }

  // allocate new data if size is given
  if ( size_ >= 0 )
  {
    data_    = new char[size_];
    alloced_ = true;
  }

  // remember we own it
  pwned_   = true;
}


ssize_t saga::impl::buffer::get_size (void) const
{
  // signal an on-demand alloc buffer
  if ( pwned_ && ! alloced_ )
  {
    return -1;
  }

  // return actual size
  return size_;
}


