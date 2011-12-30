
#ifndef SAGA_API_FILESYSTEM_DIR_HPP
#define SAGA_API_FILESYSTEM_DIR_HPP

#include <string>

#include <saga/util/shareable.hpp>

#include <saga/api/task/task.hpp>
#include <saga/api/filesystem/file.hpp>

#include <saga/impl/filesystem/dir.hpp>

namespace saga
{
  namespace filesystem
  {
    class dir
    {
      private:
        saga::util::shared_ptr <saga::impl::filesystem::dir> impl_;

      public:
        dir (std::string url);

        std::string            get_url (void);
        saga::filesystem::file open    (std::string url);
    };

  } // namespace filesystem

} // namespace saga

#endif // SAGA_API_FILESYSTEM_DIR_HPP

