
#ifndef SAGA_IMPL_FILESYSTEM_FILE_HPP
#define SAGA_IMPL_FILESYSTEM_FILE_HPP

#include <map>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <saga/util/mutex.hpp>
#include <saga/util/scoped_lock.hpp>
#include <saga/util/shareable.hpp>
#include <saga/util/enums.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

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
          int get_size (void);

          // async version of same call
          saga::util::shared_ptr <saga::impl::task> get_size (saga::impl::call_mode);

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

