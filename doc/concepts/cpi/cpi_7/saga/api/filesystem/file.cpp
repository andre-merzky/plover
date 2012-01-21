
#include "file.hpp"

namespace saga
{
  namespace filesystem
  {
    file::file (std::string url)
      : impl_ (new saga::impl::filesystem::file)
    {
      SAGA_UTIL_STACKTRACE ();
      (void) impl_->constructor (url);
    }

    file::file (saga::util::shared_ptr <saga::impl::filesystem::file> impl)
      : impl_ (impl)
    {
      SAGA_UTIL_STACKTRACE ();
      // FIXME: where to get the URL/idata?  Is constructor here really useful,
      // as impl already exists?  Is impl always initialized?
      // In general, need to clear up who is filling idata, when the object
      // is adaptor initialized / bound (void) 
      //
      // impl_->constructor (url);
    }

    void file::copy (std::string tgt)
    {
      SAGA_UTIL_STACKTRACE ();
      (void) impl_->copy (tgt);
    }

  } // namespace filesystem

} // namespace saga

