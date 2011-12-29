
#ifndef SAGA_IMPL_FILESYSTEM_DIR_HPP
#define SAGA_IMPL_FILESYSTEM_DIR_HPP

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
      // saga::impl::dir_instance_data manages the state information for one
      // specific saga::dir instance.  Note that this state is shared by all
      // dir adaptors: they can obtained a scoped-locked copy of it via
      // get_instance_data()
      class dir_instance_data : public saga::util::shareable
      {
        private:
          saga::util::mutex mtx_;
          // saga::session s_;

        public:
          bool        valid;
          std::string url;
          size_t      pos;

          void dump (std::string msg = "")
          {
            LOGSTR (DEBUG, "dir_instance_data dump") 
              << "dir_instance_data (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl
              << "    valid       : " << valid << std::endl
              << "    url         : " << url   << std::endl
              << "    pos         : " << pos   << std::endl;
          }
      };


      class dir : public saga::impl::impl_base
      {
        //////////////////////////////////////////////////////////////////////
        //
        // our dir adaptor base class
        //
        private:
          saga::util::shared_ptr <dir_instance_data>  idata_;

        public:
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

          // allow adaptor to obtain instance data (unlocked)
          saga::util::shared_ptr <dir_instance_data> get_instance_data (void)
          {
            return idata_;
          }

          void dump (std::string msg = "")
          {
            LOGSTR (DEBUG, "impl::dir dump") 
              << "impl::dir (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
            idata_.dump ("    idata_      : ");
            idata_->dump();
          }
      }; // class dir

    } // namespace filesystem

  } // namespace impl

} // namespace saga

#endif //  SAGA_IMPL_FILESYSTEM_DIR_HPP

