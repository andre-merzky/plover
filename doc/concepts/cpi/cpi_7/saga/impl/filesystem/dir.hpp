
#ifndef SAGA_IMPL_FILESYSTEM_DIR_HPP
#define SAGA_IMPL_FILESYSTEM_DIR_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/impl/filesystem/file.hpp>
#include <saga/engine/engine.hpp>

namespace saga
{
  namespace impl
  {
    namespace filesystem
    {
      class dir : public saga::impl::impl_base
      {
        //////////////////////////////////////////////////////////////////////
        //
        // our dir adaptor base class
        //
        public:
          // instance data
          // FIXME: make private
          bool        valid_;
          std::string url_;


          dir (void);

          // as the constructor is also a cpi method, and we thus want to call the
          // cpi, we want to be finished with the actual object construction,
          // and the shared_ptr setup.  Thus impl construction is rendered as
          // a two-step process
          void_t constructor (std::string url);

          // instead of get_size calling each cpi individually, the call invocation
          // is passed off to the engine
          std::string get_url (void);

          saga::util::shared_ptr <saga::impl::filesystem::file> open (std::string url);

          void dump (std::string msg = "")
          {
            LOGSTR (DEBUG, "impl::dir dump") 
              << "impl::dir (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl
              << "    valid       : " << valid_ << std::endl
              << "    url         : " << url_   << std::endl;
          }
      }; // class dir

    } // namespace filesystem

  } // namespace impl

} // namespace saga

#endif //  SAGA_IMPL_FILESYSTEM_DIR_HPP

