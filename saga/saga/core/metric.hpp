
#ifndef SAGA_SAGA_METRIC_HPP
#define SAGA_SAGA_METRIC_HPP

#include <string> 

#include <saga/util/shared_ptr.hpp>

#include <saga/saga/core/object.hpp>
#include <saga/saga/core/attributes.hpp>

namespace saga
{
  class metric : public object, 
                 public attributes
  {
    friend class saga::impl::metric;

    protected:
      // create metric from impl
      metric (saga::impl::metric * impl);

      // create metric from impl
      metric (saga::util::shared_ptr <saga::impl::metric> impl);

      // get impl from base classes
      saga::util::shared_ptr <saga::impl::metric> get_obj_impl  (void) const;
      saga::util::shared_ptr <saga::impl::metric> get_attr_impl (void) const;

    public:
      metric  (void); // FIXME
      ~metric (void);

      // copy c'tor
      metric  (const saga::metric & src);
  };
}

#endif // SAGA_SAGA_METRIC_HPP

