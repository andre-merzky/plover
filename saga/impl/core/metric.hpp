
#ifndef SAGA_IMPL_METRIC_HPP
#define SAGA_IMPL_METRIC_HPP

#include <saga/impl/core/object.hpp>
#include <saga/impl/core/attributes.hpp>

// FIXME: this needs to be in impl
namespace saga
{
  namespace impl
  {
    class metric : public object, 
                   public attributes
    {
      public:
        metric  (std::set <std::string> attrib_ro_s, 
                 std::set <std::string> attrib_ro_v, 
                 std::set <std::string> attrib_rw_s, 
                 std::set <std::string> attrib_rw_v, 
                 bool                   extensible);
        ~metric (void);

        // from object interface
        // saga::metric  clone   (void) const;
    };
  }
}

#endif // SAGA_IMPL_METRIC_HPP

