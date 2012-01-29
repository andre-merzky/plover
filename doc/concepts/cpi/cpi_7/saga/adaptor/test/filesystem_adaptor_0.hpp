
#ifndef SAGA_ADAPTOR_FILESYSTEM_0_HPP
#define SAGA_ADAPTOR_FILESYSTEM_0_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/engine/engine.hpp>

#include <saga/cpi/filesystem/file.hpp>
#include <saga/cpi/filesystem/dir.hpp>

#include <saga/impl/filesystem/file.hpp>
#include <saga/impl/filesystem/dir.hpp>

namespace saga
{
  namespace adaptor
  {
    namespace test // test adaptprs
    {
      class file_adaptor_0 : public saga::impl::filesystem::file_cpi
      {
        private:
          typedef saga::impl::void_t                         void_t;
          typedef saga::impl::filesystem::file               api_t;
          typedef saga::impl::filesystem::file_cpi           cpi_t;

        public:
          file_adaptor_0    (void) { } 
          ~file_adaptor_0   (void) { } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc, 
                            std::string                                       url);
          void copy        (saga::util::shared_ptr <saga::impl::call_context> cc,
                            std::string                                       tgt);
          void get_size    (saga::util::shared_ptr <saga::impl::call_context> cc);
      };



      class dir_adaptor_0 : public saga::impl::filesystem::dir_cpi
      {
        private:
          typedef saga::impl::void_t                        void_t;
          typedef saga::impl::filesystem::dir               api_t;
          typedef saga::impl::filesystem::dir_cpi           cpi_t;

        public:
          dir_adaptor_0    (void);
          ~dir_adaptor_0   (void);
          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                            std::string                                       url);
          void get_url     (saga::util::shared_ptr <saga::impl::call_context> cc);
      };

    } // namespace test

  } // namespace adaptor

} // namespace saga

#endif //  SAGA_ADAPTOR_FILESYSTEM_0_HPP

