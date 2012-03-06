
#ifndef SAGA_API_FILESYSTEM_FILE_HPP
#define SAGA_API_FILESYSTEM_FILE_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/api/async/task.hpp>
#include <saga/impl/filesystem/file.hpp>

namespace saga
{
  namespace filesystem
  {
    class file
    {
      private:
        saga::util::shared_ptr <saga::impl::filesystem::file> impl_;

      public:
      // void constructor
      file                 (void)
          : impl_ (NULL)
      {
      }
        file (saga::util::shared_ptr <saga::impl::filesystem::file> impl)
          : impl_ (impl)
        {
        }


        file (std::string url)
          : impl_ (new saga::impl::filesystem::file)
        {
          (void) impl_->constructor (url);
        }

        void copy (std::string tgt)
        {
          (void) impl_->copy (tgt);
        }

        size_t get_size (void)
        {
          return (get_size <saga::async::Sync> ().get_result <size_t> ());
        }

        template <enum saga::async::mode M>
        saga::async::task get_size (void)
        {
          return saga::async::task (impl_->get_size (M));
        }

    };

  } // namespace filesystem

} // namespace saga

#endif //  SAGA_API_FILESYSTEM_FILE_HPP

