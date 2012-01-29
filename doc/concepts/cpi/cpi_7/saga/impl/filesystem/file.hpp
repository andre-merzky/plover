
#ifndef SAGA_IMPL_FILESYSTEM_FILE_HPP
#define SAGA_IMPL_FILESYSTEM_FILE_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/impl/async/task.hpp>
#include <saga/engine/engine.hpp>

namespace saga
{
  namespace impl
  {
    namespace filesystem
    {
      // saga impl class
      class file : public saga::impl::impl_base
      {
        public:
          // FIXME: valid_ could go into impl_base, as most/all impl's will have to
          // step construction/destruction
          bool        valid_;

          // instance data
          // FIXME: make private
          std::string url_;
          size_t      pos_;


          file (void);

          // as the constructor is also a cpi method, and we thus want to call the
          // cpi, we want to be finished with the actual object construction,
          // and the shared_ptr setup.  Thus impl construction is rendered as
          // a two-step process
          void_t constructor (std::string url);

          // instead of get_size calling each cpi individually, the call invocation
          // is passed off to the engine
          // We have *only* async versions of the API call.
          saga::util::shared_ptr <saga::impl::async::task> get_size (saga::async::mode);

          // other calls for copy
          void_t copy (std::string tgt);

          void dump (std::string msg = "")
          {
            LOGSTR (DEBUG, "impl::file dump") 
              << "impl::file (" << this << ") : " 
              << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl
              << "    valid       : " << valid_ << std::endl
              << "    url         : " << url_   << std::endl
              << "    pos         : " << pos_   << std::endl;
          }

      }; // class file

    } // namespace filesystem

  } // namespace impl

} // namespace saga

#endif //  SAGA_IMPL_FILESYSTEM_FILE_HPP

