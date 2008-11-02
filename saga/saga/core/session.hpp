
#ifndef SAGA_SAGA_SESSION_HPP
#define SAGA_SAGA_SESSION_HPP

#include <saga/util/shared_ptr.hpp>

#include <saga/saga/core/object.hpp>

namespace saga
{
  class session : public object
  {
    friend class saga::object;
    friend class saga::impl::object;
    friend class saga::impl::session;

    public:
      // default session impl
      static saga::impl::session * none_;
      static saga::impl::session * default_;

      // create session from impl
      session (saga::impl::session * impl);

      session  (void);
      ~session (void);

      // copy c'tor
      session  (const saga::session & src);

      static session no_session (void);
      static session default_session (void);

    protected:
      // get impl from base class
      saga::util::shared_ptr <saga::impl::session> get_obj_impl (void) const;

      // c'tor from impl
      session (saga::util::shared_ptr <saga::impl::session> impl);
  };
}


#endif // SAGA_SAGA_SESSION_HPP

