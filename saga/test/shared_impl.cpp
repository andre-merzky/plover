
#include <saga/util/shared_ptr.hpp>

namespace saga
{
  namespace impl
  {
    ///////////////////////////////////////////////////
    class base
    {
      public:
        base () {}
        ~base () {}
    };

    ///////////////////////////////////////////////////
    class dved : public saga::impl::base
    {
      public:
        dved () {}
        ~dved () {}

      saga::util::shared_ptr <saga::impl::base> get_base_impl ()
      {
        saga::impl::base * b = static_cast <saga::impl::base *> (this);

        return saga::util::shared_ptr <saga::impl::base> (b);
      }

    };
  }

  ///////////////////////////////////////////////////
  class dved;
  class base
  {
    private:
      saga::util::shared_ptr <saga::impl::base> impl_;
    
    public:
      base () {}
      base (saga::util::shared_ptr <saga::impl::base> impl)
        : impl_ (impl)
      {}
      base (saga::util::shared_ptr <saga::impl::dved> impl)
        : impl_ (impl->get_base_impl ())
      {}
      ~base () {}

      saga::util::shared_ptr <saga::impl::base> get_base_impl ()
      {
        return impl_;
      }
  };

  ///////////////////////////////////////////////////
  class dved : public saga::base
  {
    public:
      dved () {}
      dved (saga::util::shared_ptr <saga::impl::dved> impl)
        : saga::base (impl)
      {
      }
      ~dved () {}

      saga::util::shared_ptr <saga::impl::base> get_base_impl (void) const
      { 
        // from where should we get the impl?  (base class)
        typedef saga::base base_type;

        // what type should the impl have?
        typedef saga::impl::dved impl_type;

        // get impl from base class, and cast into correct type
        return boost::static_pointer_cast <impl_type> (this->base_type::get_base_impl ());
      }

  };
}


///////////////////////////////////////////////////
int main ()
{
  saga::base b;
  saga::dved d;

  return (0);
}

