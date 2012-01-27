
#ifndef SAGA_UTIL_SINGLETON_HPP
#define SAGA_UTIL_SINGLETON_HPP

#include <string>
#include <vector>
#include <stdlib.h> // FIXME: for abort

#include <saga/util/demangle.hpp>

namespace saga
{
  namespace util
  {
    // forwad declaration
    template <typename T>
    class singleton;


    /// // the singleton destroyer class will get ownership of the singleton
    /// // instance, and take care of it's deletion.
    /// template <typename T>
    /// class singleton_destroyer 
    /// {
    ///   private:
    ///     T * s_;
    ///
    ///     // singleton_destroyers are not to be copied around etc.
    ///     singleton_destroyer             (singleton_destroyer const &);
    ///     singleton_destroyer & operator= (singleton_destroyer const &);
    ///
    ///   public:
    ///     singleton_destroyer  (T * s = 0)
    ///     {
    ///       // s_ = s;
    ///       //
    ///       // FIXME:
    ///       // For some reason, this dtor seems to get called *after* get, and
    ///       // just before the d'tor - so any created instance is lost by setting
    ///       // s_ to 0 (the default), which causes a mem-leak.  So, the code now
    ///       // relies on the fact that s_ is, by default, initialized to NULL,
    ///       // which I don't think is guaranteed.
    ///     }
    ///
    ///     ~singleton_destroyer (void)
    ///     {
    ///       // fixme: not sure whats up... :-(
    ///       /// delete s_;
    ///     }
    ///
    ///     void set (T * s)
    ///     {
    ///       s_ = s;
    ///     }
    ///
    /// };

    // our actual singleton base class
    template <typename T>
    class singleton
    {
      public:
        static T * get_singleton (void)
        {
          if ( ! singleton_instance_ ) 
          {
            singleton_instance_ = new T;
            /// singleton_destroyer_.set (singleton_instance_);
          }

          return singleton_instance_;
        }

      private:
        // this class should be used, not inherited.
        singleton (void);
        ~singleton (void)
        {
          std::cout << " singleton d'tor ######################### should happen last" << std::endl;
        };

        /// friend class singleton_destroyer <T>;

      
      private:
        // singletons are not to be copied around etc.
        singleton             (singleton const &);
        singleton & operator= (singleton const &);

        // this should be the *only* instance of singleton class T.
        static T * singleton_instance_;

        // static members get destroyed by C++, unlike static pointer members
        // like the singleton instance above.  So, we delegate the instance
        // deletion to a singleton destroyer...
        /// static singleton_destroyer <T> singleton_destroyer_;
    };

    // initialized the singleton instance to NULL, so that the first call to
    // get_singleton can in fact create a valid instance.
    template <typename T> T * singleton <T>::singleton_instance_ = NULL;

    // also make sure the static destroyer instance gets created (shouldn't that
    // go into the cpp?  FIXME
    /// template <typename T>
    /// singleton_destroyer<T> singleton <T>::singleton_destroyer_;



  } // namespace saga

} // namespace util

#endif // SAGA_UTIL_SINGLETON_HPP

