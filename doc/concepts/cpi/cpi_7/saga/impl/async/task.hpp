
#ifndef SAGA_IMPL_TASK_TASK_HPP
#define SAGA_IMPL_TASK_TASK_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/api/async/state.hpp>
#include <saga/engine/engine.hpp>

namespace saga
{
  namespace impl
  {
    namespace async
    {
      class task_instance_data : public saga::util::shareable
      {
        // saga::impl::task_instance_data manages the state information for one
        // specific saga::async::task instance.  Note that this state is shared by all
        // task adaptors: they can obtained a scoped-locked copy of it via
        // get_instance_data()
        private:
          // saga::session s_;

        public:
          saga::util::shared_ptr <saga::impl::call_context> t_cc;

          void dump (std::string msg = "")
          {
            LOGSTR (DEBUG, "task_instance_data dump") 
              << "(" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl
                                          << "    t_cc        : " << std::endl;
            t_cc.dump ();
            t_cc->dump ();  // should give infinite recursion!
          }
      };


      class task : public saga::impl::impl_base
      {
        private:
          saga::util::shared_ptr <task_instance_data> idata_;

        public:
          task (void);
          task (saga::util::shared_ptr <saga::impl::call_context> cc);

          void_t                            constructor (void);
          saga::async::state                get_state   (void);
          saga::util::shared_ptr <result_t> get_result  (void);

          // allow adaptor to obtain instance data (unlocked)
          saga::util::shared_ptr <task_instance_data> get_instance_data (void)
          {
            return idata_;
          }

          virtual void dump (std::string msg = "")
          {
            LOGSTR (DEBUG, "task dump") 
              << "impl::task (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
            idata_.dump ("    idata_      : ");
            idata_->dump();
          }

      }; // class task

    } // namespace async

  } // namespace impl

} // namespace saga

#endif //  SAGA_IMPL_TASK_TASK_HPP

