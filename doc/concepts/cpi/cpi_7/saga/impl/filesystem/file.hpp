
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
      // saga::impl::file_instance_data manages the state information for one
      // specific saga::file instance.  Note that this state is shared by all
      // file adaptors: they can obtained a scoped-locked copy of it via
      // get_instance_data()
      class file_instance_data : public saga::util::shareable
      {
        private:
          // saga::session s_;

        public:
          std::string url;
          size_t      pos;
          bool        valid;

          void dump (std::string msg = "")
          {
            LOGSTR (DEBUG, "file_instance_data dump") 
              << "file_instance_data (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl
              << "    valid       : " << valid << std::endl
              << "    url         : " << url   << std::endl
              << "    pos         : " << pos   << std::endl;
          }
      };


      // saga impl class
      class file : public saga::impl::impl_base
      {
        private:
          saga::util::shared_ptr <file_instance_data> idata_;

        public:
          file (void);

          // allow adaptor to obtain instance data (unlocked)
          saga::util::shared_ptr <file_instance_data> get_instance_data (void)
          {
            return idata_;
          }

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
              << "impl::file (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
            idata_.dump  ("    idata_      : ");
            idata_->dump ();
            impl_base::dump ();
          }

      }; // class file

    } // namespace filesystem

  } // namespace impl

} // namespace saga

#endif //  SAGA_IMPL_FILESYSTEM_FILE_HPP

