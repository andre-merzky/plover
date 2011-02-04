
#ifndef SAGA_SAGA_CONTEXT_HPP
#define SAGA_SAGA_CONTEXT_HPP

#include <saga/util/shared_ptr.hpp>

#include <saga/saga/core/object.hpp>
#include <saga/saga/core/attributes.hpp>

namespace saga
{
  namespace impl
  {
    class context;
  }

  class context : public object, 
                  public attributes
  {
    typedef saga::impl::context impl_type;

    friend class saga::impl::context;


    protected:
      // create context from impl
      context (                        impl_type * impl);
      context (saga::util::shared_ptr <impl_type>  impl);

      // get impl from base classes
      template <class base_type>
      saga::util::shared_ptr <impl_type> get_impl (void) const;


    public:
      context  (void);
      ~context (void);

      // copy c'tor
      context  (const saga::context & src);
  };
}


#endif // SAGA_SAGA_CONTEXT_HPP

