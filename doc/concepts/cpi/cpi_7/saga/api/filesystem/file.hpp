
#ifndef SAGA_API_FILESYSTEM_FILE_HPP
#define SAGA_API_FILESYSTEM_FILE_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/api/task/task.hpp>
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
        file (std::string url);
        file (saga::util::shared_ptr <saga::impl::filesystem::file> impl);

        void copy    (std::string tgt);
        int get_size (void);

        template <enum saga::impl::call_mode M>
        saga::task get_size (void)
        {
          SAGA_UTIL_STACKTRACE ();
          return saga::task (impl_->get_size (M));
        }

    };

  } // namespace filesystem

} // namespace saga

#endif //  SAGA_API_FILESYSTEM_FILE_HPP

