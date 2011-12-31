
#ifndef SAGA_CPI_TASK_TASK_HPP
#define SAGA_CPI_TASK_TASK_HPP

#include <saga/util/shareable.hpp>
#include <saga/engine/engine.hpp>

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
      };

    } // namespace async

  } // namespace impl

} // namespace saga

#endif //  SAGA_CPI_TASK_TASK_HPP

