
#ifndef SAGA_IMPL_TASK_TASK_HPP
#define SAGA_IMPL_TASK_TASK_HPP

#include <map>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <saga/util/mutex.hpp>
#include <saga/util/scoped_lock.hpp>
#include <saga/util/shareable.hpp>
#include <saga/util/enums.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

namespace saga
{
  namespace impl
  {
    class task_instance_data : public saga::util::shareable
    {
      // saga::impl::task_instance_data manages the state information for one
      // specific saga::task instance.  Note that this state is shared by all
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
        saga::impl::call_state            get_state   (void);
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

  } // namespace impl

} // namespace saga

#endif //  SAGA_IMPL_TASK_TASK_HPP

