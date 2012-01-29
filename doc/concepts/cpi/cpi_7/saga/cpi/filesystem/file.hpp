
#ifndef SAGA_CPI_FILESYSTEM_FILE_HPP
#define SAGA_CPI_FILESYSTEM_FILE_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/api/async/state.hpp>
#include <saga/engine/cpi_base.hpp>
#include <saga/engine/call_context.hpp>

namespace saga
{
  namespace impl
  {
    namespace filesystem
    {
      class file_cpi : public saga::impl::cpi_base
      {
        public:
          virtual ~file_cpi (void);

          virtual void constructor (saga::util::shared_ptr <call_context> cc, 
                                    std::string                           url); 
          virtual void copy        (saga::util::shared_ptr <call_context> cc, 
                                    std::string                           tgt);

          //  the get_size method for the async versions.  The adaptor needs to
          //  switch over the cc's mode enum to see what async flavor is wanted
          //  / needed, but the returned task's state can also be adjusted by
          //  the calling func or by the engine. (run()/wait())
          virtual void get_size    (saga::util::shared_ptr <call_context> cc);
      };

    } // namespace filesystem

  } // namespace impl

} // namespace saga

#endif //  SAGA_CPI_FILESYSTEM_FILE_HPP

