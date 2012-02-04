
#ifndef SAGA_CPI_TASK_TASK_HPP
#define SAGA_CPI_TASK_TASK_HPP

#include <saga/util/shareable.hpp>
#include <saga/engine/cpi_base.hpp>
#include <saga/engine/call_context.hpp>

namespace saga
{
  namespace impl
  {
    namespace async
    {
      class task_cpi : public saga::impl::cpi_base
      {
        public:
          virtual ~task_cpi (void);

          virtual void constructor (saga::util::shared_ptr <call_context> cc);
          virtual void get_state   (saga::util::shared_ptr <call_context> cc);
          virtual void get_result  (saga::util::shared_ptr <call_context> cc);
          virtual void run         (saga::util::shared_ptr <call_context> cc);
          virtual void wait        (saga::util::shared_ptr <call_context> cc);
      };

    } // namespace async

  } // namespace impl

} // namespace saga

#endif //  SAGA_CPI_TASK_TASK_HPP

