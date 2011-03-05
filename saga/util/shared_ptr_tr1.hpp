
#ifndef SAGA_UTIL_SHAREDPTRTR1_HPP
#define SAGA_UTIL_SHAREDPTRTR1_HPP

#ifndef SAGA_USE_TR1
# warning "this include should only be used with TR1 defined"
#endif

// #warning "using shared_ptr implementation from TR1"

#include <saga/util/demangle.hpp>

#include <tr1/memory>
#include <string>
#include <iostream>

namespace saga
{
  namespace util
  {
    // declaration as std::shared_ptr derivate
    template <class T>
    class shared_ptr : public std::tr1::shared_ptr <T>
    {
      // friend myself for allowing me to use the private c'tor in the
      // get_shared_ptr impl
      template <class U>
      friend class saga::util::shared_ptr;


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
      saga::util::shared_ptr <U> get_shared_ptr (void)
      {
        saga::util::shared_ptr <U> ret (std::tr1::dynamic_pointer_cast <U> (*this));

        return ret;
      }

      // is_a() is related to get_shared_ptr<U>, as it checks if get_shared_ptr<U>, and in
      // particular the casting, would succeed.
      template <typename U>
      bool is_a (void)              
      {
        saga::util::shared_ptr <U> ret (std::tr1::dynamic_pointer_cast <U> (*this));

        if ( ret )
        {
          return true;
        }
        else
        {
          return false;
        }
      }

      saga::util::shared_ptr <T> get_shared_ptr (void)
      {
        saga::util::shared_ptr <T> ret (*this);
        return ret;
      }

      long get_count (void)
      {
        return this->use_count ();
      }

      std::string get_ptype (void)
      {
        if ( NULL == this->get () )
        {
          return "NULL";
        }

        return (typeid (*(this->get ())).name ());
      }

      std::string get_ptype_demangled (void)
      {
        return (saga::util::demangle (get_ptype ()));
      }

    };

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_SHAREDPTRTR1_HPP

