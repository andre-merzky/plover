
#ifndef SAGA_SAGA_MONITORING_HPP
#define SAGA_SAGA_MONITORING_HPP

#include <list>
#include <string>

#include <saga/util/shared_ptr.hpp>


// do we need async version?

namespace saga
{
  namespace impl
  {
    class monitorable;
  }

  class monitorable 
  {
    friend class impl::monitorable;

    private:
    saga::util::shared_ptr <saga::impl::monitorable> monitor_impl_;

    protected:
      monitorable (void);

      // copy c'tor (shallow copy)
      monitorable (const monitorable & src);

      // copy c'tor (from impl, for inheritance)
      monitorable (impl::monitorable * impl);
      monitorable (saga::util::shared_ptr <impl::monitorable> impl);

      ~monitorable (void);

    public:
      metric      get_metric                (std::string name);
      std::list <std::string>
                  list_metrics              (void);

      int         add_callback              (std::string name,
                                             callback    cb);
      void        remove_callback           (int         cookie);

    protected:
      // friends can see the impl_
      saga::util::shared_ptr <impl::monitorable> get_monitor_impl (void) const;
  };
}

#endif // SAGA_SAGA_MONITORING_HPP

