
#ifndef SAGA_ADAPTOR_TASK_0_HPP
#define SAGA_ADAPTOR_TASK_0_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/cpi/task/task.hpp>
#include <saga/impl/task/task.hpp>

#include <saga/engine/engine.hpp>

namespace saga
{
  namespace adaptor
  {
    namespace test // test adaptprs
    {
      class task_adaptor_0 : public saga::impl::task_cpi
      {
        private:
          typedef saga::impl::void_t             void_t;
          typedef saga::impl::task               api_t;
          typedef saga::impl::task_cpi           cpi_t;
          typedef saga::impl::task_instance_data idata_t;

          static void * threaded_cc (void * cc_sp);

        public:
          task_adaptor_0  (void);
          ~task_adaptor_0 (void);

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc);
          void get_state   (saga::util::shared_ptr <saga::impl::call_context> cc);
          void get_result  (saga::util::shared_ptr <saga::impl::call_context> cc);
      };

    } // namespace test

  } // namespace adaptor

} // namespace saga

#endif //  SAGA_ADAPTOR_TASK_0_HPP

