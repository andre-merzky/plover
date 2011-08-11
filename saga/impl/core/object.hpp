
#ifndef SAGA_IMPL_OBJECT_H
#define SAGA_IMPL_OBJECT_H

#include <string>

#include <saga/util/shared_ptr.hpp>

#include <saga/saga/core/object.hpp>
#include <saga/saga/core/session.hpp>

namespace saga
{
  namespace impl
  {
    class object : public saga::util::shareable
    {
      friend class saga::object;

      private:
        std::string           id_;
        saga::object::type    type_;

        saga::util::shared_ptr<saga::impl::session> session_;

        std::string create_uuid (void);

        // we want polymorphism, so that dynamic casts work up and down the class
        // hierarchy.  Thus we add one virtual method.
        virtual bool is_polymorphic (void) { return true; }

      protected:
        void set_session (saga::session session);

      public:
        object (saga::object::type type);
        virtual ~object (void);

        std::string        get_id      (void) const;
        saga::session      get_session (void) const;
        saga::object::type get_type    (void) const;

        saga::object       clone       (void) const;
    };
  }
}

#endif // SAGA_IMPL_OBJECT_H

