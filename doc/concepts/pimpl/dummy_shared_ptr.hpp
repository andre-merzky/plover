
#ifndef DUMMY_SHARED_PTR_HPP
#define DUMMY_SHARED_PTR_HPP


#ifdef USE_BOOST 

# include <boost/shared_ptr.hpp>

#else // USE_BOOST

# include <saga/util/shared_ptr.hpp>

#endif // USE_BOOST


namespace dummy
{
#ifdef USE_BOOST
  // declaration as boost shared_ptr derivate
  template <class T>
  class shared_ptr : public boost::shared_ptr <T>
  {
    // friend myself for allowing me to use the private c'tor in the
    // get_shared_ptr impl
    template <class U>
    friend class dummy::shared_ptr;

    private:
      // only used for get_shared_ptr()
      shared_ptr (boost::shared_ptr <T> impl)
        : boost::shared_ptr <T> (impl)
      {
      }

    public:
      shared_ptr (void)
        : boost::shared_ptr <T> ()
      {
      }

      shared_ptr (T * impl)
        : boost::shared_ptr <T> (impl)
      {
      }

      template <class U>
      dummy::shared_ptr <U> get_shared_pointer (void)
      {
        dummy::shared_ptr <U> ret (boost::dynamic_pointer_cast <U> (*this));
        return ret;
      }
  };

#else // ! USE_BOOST

  // forward decl for the get_shared_ptr
  template <class T> class shared_ptr;

  // declaration as saga::util shared_ptr derivate
  template <class T>
  class shared_ptr : public saga::util::shared_ptr <T>
  {
    // friend myself for allowing me to use the private c'tor in the
    // get_shared_ptr impl
    template <class U>
    friend class dummy::shared_ptr;

    private:
      // only used for get_shared_ptr()
      shared_ptr (saga::util::shared_ptr <T> impl)
        : saga::util::shared_ptr <T> (impl)
      {
      }


    public:
      shared_ptr (void)
        : saga::util::shared_ptr <T> ()
      {
      }

      shared_ptr (T * impl)
        : saga::util::shared_ptr <T> (impl)
      {
      }

      template <class U>
      dummy::shared_ptr <U> get_shared_pointer (void)
      {
        // I am not really sure why I need to add the template keyword here, but
        // see http://stackoverflow.com/questions/4929869/c-calling-template-functions-of-base-class
        // looks ugly, doesn't it?
        dummy::shared_ptr <U> ret = this->template get_shared_ptr <U> ();
        return ret;
      }
  };
#endif // USE_BOOST
}


#endif // DUMMY_SHARED_PTR_HPP

