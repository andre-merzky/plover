
#ifndef SAGA_IMPL_MONITORABLE_H
#define SAGA_IMPL_MONITORABLE_H

#include <map>
#include <list>
#include <string>
#include <utility>

#include <saga/saga/core/metric.hpp>
#include <saga/saga/core/callback.hpp>

namespace saga
{
  namespace impl
  {
    class monitorable 
    {
      private:
        int cb_count_;

        std::map <std::string, saga::metric>                     metrics_;
        std::map <int, std::pair <std::string, saga::callback> > callbacks_;

        bool metric_exists (std::string name, 
                            bool        except = true);

      public:
        monitorable  (void); // FIXME: get initial metrics, and bool extensible
        ~monitorable (void);

        saga::metric get_metric               (std::string    name);
        std::list <std::string>
                     list_metrics             (void);

        int         add_callback              (std::string    name, 
                                               saga::callback cb);
        void        remove_callback           (int            cookie);
    };
  }
}

#endif // SAGA_IMPL_MONITORABLE_H

