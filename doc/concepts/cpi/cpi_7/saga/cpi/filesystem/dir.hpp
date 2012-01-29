
#ifndef SAGA_CPI_FILESYSTEM_DIR_HPP
#define SAGA_CPI_FILESYSTEM_DIR_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/engine/cpi_base.hpp>
#include <saga/engine/call_context.hpp>

namespace saga
{
  namespace impl
  {
    namespace filesystem
    {
      class dir_cpi : public saga::impl::cpi_base
      {
        public:
          virtual ~dir_cpi (void);

          virtual void constructor (saga::util::shared_ptr <call_context> cc, 
                                    std::string                           url);
          virtual void get_url     (saga::util::shared_ptr <call_context> cc);
          virtual void open        (saga::util::shared_ptr <call_context> cc, 
                                    std::string                           url);
      };

    } // namespace filesystem

  } // namespace impl

} // namespace saga

#endif //  SAGA_CPI_FILESYSTEM_DIR_HPP

