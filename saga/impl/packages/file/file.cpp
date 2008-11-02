
#include <saga/impl/packages/file/file.hpp>
#include <saga/impl/object.hpp>

#include <saga/saga/packages/file/file.hpp>

saga::impl::file::file (void)
  : object   (saga::object::File)
{
}

saga::file saga::impl::file::clone (void) const
{
  saga::impl::file * tgt = new saga::impl::file ();
  
  // copy all contexts to new file

  return saga::file (tgt);
}


