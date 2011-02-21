
#ifndef DUMMY_SHARED_PTR_HPP
#define DUMMY_SHARED_PTR_HPP


#ifdef USE_TR1 

# include <tr1/memory>
# include <string>

#else // USE_TR1

# include <saga/util/shared_ptr.hpp>

#endif // USE_TR1


namespace dummy
{
#ifdef USE_TR1
  // declaration as std::shared_ptr derivate
  template <class T>
  class shared_ptr : public std::tr1::shared_ptr <T>
  {
    // friend myself for allowing me to use the private c'tor in the
    // get_shared_ptr impl
    template <class U>
    friend class dummy::shared_ptr;

    private:
      // only used for get_shared_ptr()
      shared_ptr (std::tr1::shared_ptr <T> impl)
        : std::tr1::shared_ptr <T> (impl)
      {
      }

    public:
      shared_ptr (void)
        : std::tr1::shared_ptr <T> ()
      {
      }

      shared_ptr (T * impl)
        : std::tr1::shared_ptr <T> (impl)
      {
      }

      template <class U>
      dummy::shared_ptr <U> get_shared_pointer (void)
      {
        dummy::shared_ptr <U> ret (std::tr1::dynamic_pointer_cast <U> (*this));
        return ret;
      }

      std::string get_ptype (void)
      {
        if ( NULL == this->get () )
        {
          return "NULL";
        }

        return (typeid (this->get ()).name ());
      }

  };

#else // ! USE_TR1

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

      long use_count (void) const
      {
        return this->get_count ();
      }
  };
#endif // USE_TR1
}


#endif // DUMMY_SHARED_PTR_HPP

