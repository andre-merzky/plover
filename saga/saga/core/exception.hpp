
#ifndef SAGA_SAGA_EXCEPTION_HPP
#define SAGA_SAGA_EXCEPTION_HPP

#include <string>

#include <saga/util/shared_ptr.hpp>

namespace saga
{
  // FIXME: inherit std:: exception
  class exception : public std::exception
  {
    private:
      saga::util::shared_ptr <saga::impl::exception> impl_;

    protected:
      exception (std::string msg);

    public:
      std::string get_msg (void) const;
  };

  class NotImplemented                  : public saga::exception { 
        NotImplemented       (std::string msg) : saga::exception (msg) { }; }; 
  class IncorrectURL                    : public saga::exception { 
        IncorrectURL         (std::string msg) : saga::exception (msg) { }; }; 
  class BadParameter                    : public saga::exception { 
        BadParameter         (std::string msg) : saga::exception (msg) { }; }; 
  class AlreadyExists                   : public saga::exception { 
        AlreadyExists        (std::string msg) : saga::exception (msg) { }; }; 
  class DoesNotExist                    : public saga::exception { 
        DoesNotExist         (std::string msg) : saga::exception (msg) { }; }; 
  class IncorrectState                  : public saga::exception { 
        IncorrectState       (std::string msg) : saga::exception (msg) { }; }; 
  class PermissionDenied                : public saga::exception { 
        PermissionDenied     (std::string msg) : saga::exception (msg) { }; }; 
  class AuthorizationFailed             : public saga::exception { 
        AuthorizationFailed  (std::string msg) : saga::exception (msg) { }; }; 
  class AuthenticationFailed            : public saga::exception { 
        AuthenticationFailed (std::string msg) : saga::exception (msg) { }; }; 
  class Timeoutmeter                    : public saga::exception { 
        Timeoutmeter         (std::string msg) : saga::exception (msg) { }; }; 
  class NoSuccess                       : public saga::exception { 
        NoSuccess            (std::string msg) : saga::exception (msg) { }; }; 

}

#endif // SAGA_SAGA_EXCEPTION_HPP

