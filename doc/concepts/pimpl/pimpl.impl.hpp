
#ifndef PIMPL_IMPL_HPP
#define PIMPL_IMPL_HPP

#include "pimpl.impl.ref.hpp"

namespace saga
{
  // impl classes
  namespace impl
  {
    class object : public refcount
    {
      public:
         object      () { std::cout << "c'tor impl::object      () \t- " << this << "\n"; }
        ~object      () { std::cout << "d'tor impl::object      () \t- " << this << "\n"; }
         void check  () { std::cout << "check impl::object      () \t- " << this << "\n"; }
    };

    class attributes : public refcount
    {
      public:
         attributes  () { std::cout << "c'tor impl::attributes  () \t- " << this << "\n"; }
        ~attributes  () { std::cout << "d'tor impl::attributes  () \t- " << this << "\n"; }
         void check  () { std::cout << "check impl::attributes  () \t- " << this << "\n"; }
    };

    class monitorable : public refcount
    {
      public:
         monitorable () { std::cout << "c'tor impl::monitorable () \t- " << this << "\n"; }
        ~monitorable () { std::cout << "d'tor impl::monitorable () \t- " << this << "\n"; }
         void check  () { std::cout << "check impl::monitorable () \t- " << this << "\n"; }
    };

    class ns_dir : public saga::impl::object
    {
      public:
         ns_dir     () { std::cout << "c'tor impl::ns_dir       () \t- " << this << "\n"; }
        ~ns_dir     () { std::cout << "d'tor impl::ns_dir       () \t- " << this << "\n"; }
         void check () { std::cout << "check impl::ns_dir       () \t- " << this << "\n"; }
    };

    class ns_file : public saga::impl::object, 
                    public saga::impl::attributes
    {
      public:
         ns_file    () { std::cout << "c'tor impl::ns_file      () \t- " << this << "\n"; }
        ~ns_file    () { std::cout << "d'tor impl::ns_file      () \t- " << this << "\n"; }
         void check () { std::cout << "check impl::ns_file      () \t- " << this << "\n"; }
    };

    class ns_pipe : public saga::impl::ns_file, 
                    public saga::impl::monitorable
    {
      public:
         ns_pipe    () { std::cout << "c'tor impl::ns_pipe      () \t- " << this << "\n"; }
        ~ns_pipe    () { std::cout << "d'tor impl::ns_pipe      () \t- " << this << "\n"; }
         void check () { std::cout << "check impl::ns_pipe      () \t- " << this << "\n"; }
    };


  } // namespace impl

} // namespace saga

#endif // PIMPL_IMPL_HPP

