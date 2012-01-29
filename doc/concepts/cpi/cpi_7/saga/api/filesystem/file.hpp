
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
        file (std::string url);
        file (saga::util::shared_ptr <saga::impl::filesystem::file> impl);

        void copy    (std::string tgt);

        int get_size (void)
        {
          // NOTE: the two lines below MUST be semantically equivalent to the
          // sync call.
          saga::async::task t = get_size <saga::async::Sync> ();
          int ret = t.get_result <int> ();

          LOGSTR (DEBUG, "file api get_size") << "size: " << ret << std::endl;

          return ret;
        }

        template <enum saga::async::mode M>
        saga::async::task get_size (void)
        {
          SAGA_UTIL_STACKTRACE ();

          saga::util::shared_ptr <saga::impl::async::task> t_impl = impl_->get_size (M);

          return saga::async::task (t_impl);
        }

    };

  } // namespace filesystem

} // namespace saga

#endif //  SAGA_API_FILESYSTEM_FILE_HPP

