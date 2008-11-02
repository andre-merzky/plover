
#ifndef SAGA_UTIL_ASSIGN_HPP
#define SAGA_UTIL_ASSIGN_HPP

#include <set>

namespace saga
{
  namespace util
  {
    template <class T> class assign_set : public std::set <T>
    {
      public:
        assign_set (T t)
        {
          this->push_back (t);
        }

        assign_set & operator() (T const & t)
        {
          this->push_back (t);
          return *this;
        }
    };

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_ASSIGN_HPP

