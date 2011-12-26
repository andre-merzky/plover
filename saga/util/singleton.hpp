
#ifndef SAGA_UTIL_SINGLETON_HPP
#define SAGA_UTIL_SINGLETON_HPP

#include <string>
#include <vector>

namespace saga
{
  namespace util
  {
    template <class T>
    class singleton
    {
      public:
        static T * get_singleton (void)
        {
          if ( ! singleton_instance_ ) 
          {
            singleton_instance_ = new T;
          }

          return singleton_instance_;
        }

      protected:
         singleton (void);
        ~singleton (void);
      
      private:
        singleton             (singleton const &);
        singleton & operator= (singleton const &);
        
        static T * singleton_instance_;
    };

    template <class T> T * singleton <T>::singleton_instance_ = NULL;

  } // namespace saga

} // namespace util

#endif // SAGA_UTIL_SINGLETON_HPP

