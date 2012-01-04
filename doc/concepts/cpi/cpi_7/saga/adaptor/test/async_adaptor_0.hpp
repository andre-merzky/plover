
#ifndef SAGA_ADAPTOR_ASYNC_0_HPP
#define SAGA_ADAPTOR_ASYNC_0_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/cpi/async/task.hpp>
#include <saga/impl/async/task.hpp>

#include <saga/engine/engine.hpp>

namespace saga
{
  namespace adaptor
  {
    namespace test // test adaptprs
    {
      class async_adaptor_0 : public saga::impl::async::task_cpi
      {
        private:
          typedef saga::impl::void_t                    void_t;
          typedef saga::impl::async::task               api_t;
          typedef saga::impl::async::task_cpi           cpi_t;
          typedef saga::impl::async::task_instance_data idata_t;

          static void * threaded_cc (void * cc_sp);

        public:
          async_adaptor_0  (void);
          ~async_adaptor_0 (void);

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc);
          void get_state   (saga::util::shared_ptr <saga::impl::call_context> cc);
          void get_result  (saga::util::shared_ptr <saga::impl::call_context> cc);
          void run         (saga::util::shared_ptr <saga::impl::call_context> cc);
      };

    } // namespace test

  } // namespace adaptor

} // namespace saga

#endif //  SAGA_ADAPTOR_ASYNC_0_HPP

