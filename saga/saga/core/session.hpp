
#ifndef SAGA_SAGA_SESSION_HPP
#define SAGA_SAGA_SESSION_HPP

#include <saga/util/shared_ptr.hpp>

#include <saga/saga/core/object.hpp>

namespace saga
{
  namespace impl
  {
    class object;
    class session;
  }

  class session : public object
  {
    typedef saga::impl::session impl_type;
    
    friend class saga::impl::session;

      // default session impl
      static impl_type * none_;
      static impl_type * default_;

    protected:
      // create session from impl
      session (                        impl_type * impl);
      session (saga::util::shared_ptr <impl_type>  impl);

      // get impl from base class
      template <class base_type>
      saga::util::shared_ptr <impl_type> get_impl (void) const;


    public:
      session  (void);
      ~session (void);

      // copy c'tor
      session  (const saga::session & src);

      static session no_session      (void);
      static session default_session (void);
  };
}


#endif // SAGA_SAGA_SESSION_HPP

