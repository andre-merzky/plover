
#ifndef SAGA_SAGA_METRIC_HPP
#define SAGA_SAGA_METRIC_HPP

#include <string> 

#include <saga/util/shared_ptr.hpp>

#include <saga/saga/core/object.hpp>
#include <saga/saga/core/attributes.hpp>

namespace saga
{
  namespace impl
  {
    class metric;
  }

  class metric : public object, 
                 public attributes
  {
    typedef saga::impl::metric impl_type;

    friend class saga::impl::metric;


    protected:
      // create metric from impl
      metric (                        impl_type * impl);
      metric (saga::util::shared_ptr <impl_type>  impl);

      // get impl from base classes
      template <class base_type>
      saga::util::shared_ptr <impl_type> get_impl (void) const;


    public:
      metric  (void); // FIXME
      ~metric (void);

      // copy c'tor
      metric  (const saga::metric & src);
  };
}

#endif // SAGA_SAGA_METRIC_HPP

