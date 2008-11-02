
#ifndef SAGA_IMPL_EXCEPTION_H
#define SAGA_IMPL_EXCEPTION_H

#include <string>

namespace saga
{
  namespace impl
  {
    class exception
    {
      private:
        std::string msg_;

      public:
        exception  (std::string msg);
        ~exception (void);

        std::string get_msg (void) const;
    };
  }
}

#endif // SAGA_IMPL_EXCEPTION_H

