
#ifndef SAGA_SAGA_TASK_HPP
#define SAGA_SAGA_TASK_HPP

#include <saga/util/shared_ptr.hpp>

#include <saga/saga/core/object.hpp>
#include <saga/saga/core/monitorable.hpp>

namespace saga
{
  class task : public object, 
               public monitorable
  {
    friend class saga::impl::task;

    protected:
      // create task from impl
      task (                   saga::impl::task * impl);
      task (saga::util::shared_ptr <saga::impl::task>  impl);

      // get impl from base classes
      saga::util::shared_ptr <saga::impl::task> get_obj_impl  (void) const;
      saga::util::shared_ptr <saga::impl::task> get_monitor_impl (void) const;

    public:
      task  (void);
      ~task (void);

      // copy c'tor
      task  (const saga::task & src);

      // member methods

      // inspection
      void         get_state  (void);
      template <typename T> T get_result (void);
      template <typename T> T get_object (void);

      // state management
      void run       (void);
      void wait      (float timeout = 0.0);
      void cancel    (float timeout = 0.0);

  };
}


#endif // SAGA_SAGA_TASK_HPP

