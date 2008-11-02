
#ifndef SAGA_SAGA_CONTEXT_HPP
#define SAGA_SAGA_CONTEXT_HPP

#include <saga/util/shared_ptr.hpp>

#include <saga/saga/core/object.hpp>
#include <saga/saga/core/attributes.hpp>

namespace saga
{
  class context : public object, 
                  public attributes
  {
    friend class saga::impl::context;

    private:
      // default context impl
      static saga::impl::context * default_;

    protected:
      // create context from impl
      context (saga::impl::context * impl);

      // create context from impl
      context (saga::util::shared_ptr <saga::impl::context> impl);

      // get impl from base classes
      saga::util::shared_ptr <saga::impl::context> get_obj_impl  (void) const;
      saga::util::shared_ptr <saga::impl::context> get_attr_impl (void) const;

    public:
      context  (void);
      ~context (void);

      // copy c'tor
      context  (const saga::context & src);

      static context default_context (void);
  };
}


#endif // SAGA_SAGA_CONTEXT_HPP

