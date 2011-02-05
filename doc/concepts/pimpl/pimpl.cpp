
#include <iostream>

#include <saga/util/shared_ptr.hpp>

using namespace saga::util;

namespace saga
{
  // impl classes
  namespace impl
  {
    class base_a
    {
      public:
         base_a ()     { std::cout << "c'tor impl::base_a    () \t- " << this << "\n"; }
        ~base_a ()     {}
         void check () { std::cout << "check impl::base_a    () \t- " << this << "\n"; }
    };

    class base_b
    {
      public:
         base_b ()     { std::cout << "c'tor impl::base_b    () \t- " << this << "\n"; }
        ~base_b ()     {}
         void check () { std::cout << "check impl::base_b    () \t- " << this << "\n"; }
    };

    class base_c
    {
      public:
         base_c ()     { std::cout << "c'tor impl::base_c    () \t- " << this << "\n"; }
        ~base_c ()     {}
         void check () { std::cout << "check impl::base_c    () \t- " << this << "\n"; }
    };

    class derived_a : public saga::impl::base_a
    {
      public:
         derived_a ()  { std::cout << "c'tor impl::derived_a () \t- " << this << "\n"; }
        ~derived_a ()  {}
         void check () { std::cout << "check impl::derived_a () \t- " << this << "\n"; }
    };

    class derived_ab : public saga::impl::base_a, 
                       public saga::impl::base_b
    {
      public:
         derived_ab () { std::cout << "c'tor impl::derived_ab () \t- " << this << "\n"; }
        ~derived_ab () {}
         void check () { std::cout << "check impl::derived_ab () \t- " << this << "\n"; }
    };

    class derived_abc : public saga::impl::derived_ab, 
                        public saga::impl::base_c
    {
      public:
         derived_abc () { std::cout << "c'tor impl::derived_abc () \t- " << this << "\n"; }
        ~derived_abc () {}
         void check  () { std::cout << "check impl::derived_abc () \t- " << this << "\n"; }
    };
  } // namespace impl


  // facade classes
  class base_a
  {
    private:
      shared_ptr <saga::impl::base_a> impl_;

    protected:
      shared_ptr <saga::impl::base_a> get_base_a_impl (void) {return impl_;}
      template <class base_t>
      shared_ptr <base_t> get_impl (void) {return impl_.dynamic_ptr_cast <base_t> ();}

    public:
      base_a () 
        : impl_ (new saga::impl::base_a)
      { 
        std::cout << "c'tor base::base_a    () \t- " << impl_.get () << "\n";
        check ();
      }

      base_a (shared_ptr <saga::impl::base_a> impl) 
        : impl_ (impl) 
      { 
        std::cout << "c'tor saga::base_a (impl) \t- " << impl_.get () << "\n";
        check ();
      }

      base_a (saga::impl::base_a * impl) 
        : impl_ (impl) 
      { 
        std::cout << "c'tor saga::base_a (pimpl) \t- " << impl_.get () << "\n";
        check ();
      }

      void check ()
      {
        std::cout << "check saga::base_a    () \t- " << impl_.get () << "\n";
        impl_->check ();
      }

     ~base_a () {}
  };

  class base_b
  {
    private:
      shared_ptr <saga::impl::base_b> impl_;

    protected:
      shared_ptr <saga::impl::base_b> get_base_a_impl (void) {return impl_;}
      template <class base_t>
      shared_ptr <base_t> get_impl (void) {return impl_.dynamic_ptr_cast <base_t> ();}

    public:
      base_b () 
        : impl_ (new saga::impl::base_b)
      { 
        std::cout << "c'tor saga::base_b    () \t- " << impl_.get () << "\n";
        check ();
      }

      base_b (shared_ptr <saga::impl::base_b> impl) 
        : impl_ (impl) 
      { 
        std::cout << "c'tor saga::base_b (impl) \t- " << impl_.get () << "\n";
        check ();
      }

      void check ()
      {
        std::cout << "check saga::base_b    () \t- " << impl_.get () << "\n";
        impl_->check ();
      }

     ~base_b () {}
  };

  class base_c
  {
    private:
      shared_ptr <saga::impl::base_c> impl_;

    protected:
      shared_ptr <saga::impl::base_c> get_base_a_impl (void) {return impl_;}
      template <class base_t>
      shared_ptr <base_t> get_impl (void) {return impl_.dynamic_ptr_cast <base_t> ();}

    public:
      base_c () 
        : impl_ (new saga::impl::base_c)
      { 
        std::cout << "c'tor saga::base_c    () \t- " << impl_.get () << "\n";
        check ();
      }

      base_c (shared_ptr <saga::impl::base_c> impl) 
        : impl_ (impl) 
      { 
        std::cout << "c'tor saga::base_c (impl) \t- " << impl_.get () << "\n";
        check ();
      }

      void check ()
      {
        std::cout << "check saga::base_c    () \t- " << impl_.get () << "\n";
        impl_->check ();
      }

     ~base_c () {}
  };


  class derived_a
    : public saga::base_a
  {
    private:
      shared_ptr <saga::impl::derived_a> impl_;
      saga::impl::base_a * bap;

    protected:
      shared_ptr <saga::impl::derived_a> get_derived_ab_impl (void) 
      {
        return impl_;
      }

      template <class derived_a>
      shared_ptr <saga::impl::derived_a> get_impl (void) 
      {
        return impl_;
      }

      template <class base_t>
      shared_ptr <base_t> get_impl (void) 
      {
        // return this->base_t::get_impl <base_t> ();
      }


    public:
      derived_a () 
        : impl_  (new saga::impl::derived_a),
          bap    (new saga::impl::base_a),
          base_a (bap)
      { 
        std::cout << "c'tor saga::derived_a () \t- " << impl_.get () << "\n";
        check ();

        saga::impl::derived_a * dap = impl_.get ();
        saga::impl::base_a    * bap = dynamic_cast <saga::impl::base_a *> (impl_.get ());
        std::cout << " dap: " << dap << "\n";
        std::cout << " bap: " << bap << "\n";
      }

      derived_a (shared_ptr <saga::impl::derived_a> impl) 
        : impl_  (impl),
          base_a (impl_)
      { 
        std::cout << "c'tor saga::derived_a (impl) \t- " << impl_.get () << "\n";
        check ();
      }

      void check ()
      {
        std::cout << "check saga::derived_a () \t- " << impl_.get () << "\n";
        impl_->check ();
      }

     ~derived_a () {}
  };

  class derived_ab 
    : public saga::base_a, 
      public saga::base_b
  {
    private:
      shared_ptr <saga::impl::derived_ab> impl_;

    protected:
      shared_ptr <saga::impl::derived_ab> get_derived_ab_impl (void) 
      {
        return impl_;
      }

      template <class derived_ab>
      shared_ptr <saga::impl::derived_ab> get_impl (void) 
      {
        return impl_;
      }

      template <class base_t>
      shared_ptr <base_t> get_impl (void) 
      {
        //return this->base_t::get_impl <base_t> ();
      }

    public:
      derived_ab () 
        : impl_  (new saga::impl::derived_ab),
          base_a (impl_),
          base_b (impl_)
      { 
        std::cout << "c'tor saga::derived_ab () \t- " << impl_.get () << "\n";
        check ();
      }

      derived_ab (shared_ptr <saga::impl::derived_ab> impl) 
        : impl_  (impl),
          base_a (impl_),
          base_b (impl_)
      { 
        std::cout << "c'tor saga::derived_ab (impl) \t- " << impl_.get () << "\n";
        check ();
      }

      void check ()
      {
        std::cout << "check saga::derived_ab () \t- " << impl_.get () << "\n";
        impl_->check ();
      }

     ~derived_ab () {}
  };

  class derived_abc
    : public saga::derived_ab, 
      public saga::base_c
  {
    private:
      shared_ptr <saga::impl::derived_abc> impl_;

    protected:
      shared_ptr <saga::impl::derived_abc> get_derived_abc_impl (void) 
      {
        return impl_;
      }

      template <class derived_abc>
      shared_ptr <saga::impl::derived_abc> get_impl (void) 
      {
        return impl_;
      }

      template <class base_t>
      shared_ptr <base_t> get_impl (void) 
      {
        //return this->base_t::get_impl <base_t> ();
      }

    public:
      derived_abc () 
        : impl_  (new saga::impl::derived_abc),
          derived_ab (impl_),
          base_c     (impl_)
      { 
        std::cout << "c'tor saga::derived_abc () \t- " << impl_.get () << "\n";
        check ();
      }

      derived_abc (shared_ptr <saga::impl::derived_abc> impl) 
        : impl_      (impl),
          derived_ab (impl_),
          base_c     (impl_)
      { 
        std::cout << "c'tor saga::derived_abc (impl) \t- " << impl_.get () << "\n";
        check ();
      }

      void check ()
      {
        std::cout << "check saga::derived_abc () \t- " << impl_.get () << "\n";
        impl_->check ();
      }

     ~derived_abc () {}
  };

} // namespace saga


int main ()
{
  try
  {
    std::cout << " === impl_a\n"; 
    saga::impl::derived_a * i_a = new saga::impl::derived_a ();
    i_a->check ();
    delete (i_a);

    std::cout << " === impl_ab\n"; 
    saga::impl::derived_ab * i_ab = new saga::impl::derived_ab ();
    i_ab->check ();
    delete (i_ab);

    std::cout << " === impl_abc\n"; 
    saga::impl::derived_abc * i_abc = new saga::impl::derived_abc ();
    i_abc->check ();
    delete (i_abc);

    std::cout << " === base_a\n"; saga::base_a b_a; b_a.check ();
    std::cout << " === base_b\n"; saga::base_b b_b; b_b.check ();
    std::cout << " === base_c\n"; saga::base_c b_c; b_c.check ();

    std::cout << " === derived_a\n";   saga::derived_a   d_a;   d_a.check ();
    std::cout << " === derived_ab\n";  saga::derived_ab  d_ab;  d_ab.check ();
    std::cout << " === derived_abc\n"; saga::derived_abc d_abc; d_abc.check ();
  }
  catch (const std::exception & e ) 
  {
    std::cerr << "catched: " << e.what () << "\n";
  }

  return 0;
}

