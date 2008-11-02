
#include <saga/saga/packages/file/file.hpp>
#include <saga/impl/packages/file/file.hpp>

// c'tor
saga::file::file (void)
  : saga::object (new saga::impl::file ())
{
}

// copy c'tor (shallow copy)
saga::file::file (saga::util::shared_ptr <saga::impl::file> impl)
  : object (impl)
{
}

// copy c'tor (shallow copy)
saga::file::file (saga::impl::file * impl)
  : saga::object (impl)
{
}

// copy c'tor (shallow copy)
saga::file::file (const file & src)
  : saga::object (src.get_obj_impl ())
{
}

// d'tor
saga::file::~file (void)
{
}

saga::impl::file * saga::file::none_    = NULL;
saga::impl::file * saga::file::default_ = NULL;

saga::file saga::file::no_file ()
{
 // return new saga file with no impl
 return saga::file (none_);
}

saga::file saga::file::default_file ()
{
 // FIXME
 //
 // // if we get called the first time, create a default file
 // if ( NULL == default_ )
 // {
 //   default_ = new saga::impl::file ();
 // 
 //   // initialize default file
 //   // default_->get_default_contexts ();
 // }
 // 
 // // return new saga file with the default impl
 return saga::file (default_);
}

saga::util::shared_ptr <saga::impl::object> saga::file::get_obj_impl (void) const
{ 
  // get impl from base class, and cast into correct type
  return this->saga::object::get_obj_impl ();
}


