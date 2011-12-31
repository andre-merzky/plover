
#ifndef SAGA_CPI_FILESYSTEM_FILE_HPP
#define SAGA_CPI_FILESYSTEM_FILE_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/api/task/state.hpp>
#include <saga/engine/engine.hpp>

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
          virtual void get_size    (saga::util::shared_ptr <call_context> cc);
          virtual void copy        (saga::util::shared_ptr <call_context> cc, 
                                    std::string                           tgt);

          //  we have a second get_size method for the async versions.  The
          //  call_mode parameter is somewhat redundant, as the cm is also
          //  stored in the cc, but it allows to use overloading in the adaptor
          //  for the various sync/async calls.  The adaptor needs to switch
          //  over the enum to see what async flavor is wanted / needed, 
          //  but the returned task's state can easily be adjusted by the
          //  calling functor or by the engine.
          virtual void get_size    (saga::util::shared_ptr <call_context> cc, 
                                    saga::task::mode                      m);
      };

    } // namespace filesystem

  } // namespace impl

} // namespace saga

#endif //  SAGA_CPI_FILESYSTEM_FILE_HPP

