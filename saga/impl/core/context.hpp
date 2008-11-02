
#ifndef SAGA_IMPL_CONTEXT_HPP
#define SAGA_IMPL_CONTEXT_HPP

#include <saga/impl/core/object.hpp>
#include <saga/impl/core/attributes.hpp>

// FIXME: this needs to be in impl
namespace saga
{
  namespace impl
  {
    class context : public saga::impl::object, 
                    public saga::impl::attributes
    {
      public:
        context  (std::set <std::string> attrib_ro_s, 
                  std::set <std::string> attrib_ro_v, 
                  std::set <std::string> attrib_rw_s, 
                  std::set <std::string> attrib_rw_v, 
                  bool                   extensible);
        ~context (void);

        // from object interface
        // saga::context  clone   (void) const;
    };
  }
}

#endif // SAGA_IMPL_CONTEXT_HPP

