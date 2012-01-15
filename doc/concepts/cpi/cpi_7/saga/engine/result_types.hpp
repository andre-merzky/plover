
#ifndef SAGA_ENGINE_RESULTTYPES_HPP
#define SAGA_ENGINE_RESULTTYPES_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

namespace saga
{
  namespace impl
  {
    // make handling void call signatures in templates easier
    class void_t 
    {
      public:
        friend std::ostream & operator<< (std::ostream & o, void_t const & void_t);
    };
  
    std::ostream & operator<< (std::ostream & o, void_t const & v);

    //////////////////////////////////////////////////////////////////
    template <typename T> class result_t_detail_;

    //////////////////////////////////////////////////////////////////
    //
    // the result type thingies allow to keep get_result template free on impl
    // level and below - the template free base class is used wherever possible, 
    // and the types result_t_detail_ is only aquired as needed.
    //
    class result_t : public saga::util::shareable
    {
      public:
        virtual ~result_t (void);

        template <typename T>
        T get (void)
        {
          return shared_this <result_t_detail_ <T> > ()->get ();
        }

        virtual void dump (std::string msg = "");
    };

    template <typename T>
    class result_t_detail_ : public result_t
    {
      private:
        T t_;

      public:
        result_t_detail_  (T t ) : t_ (t) { }
        result_t_detail_  (void)          { }
        ~result_t_detail_ (void)          { }

        // TODO: add assignment operator etc.

        void set (T t ) 
        {
          SAGA_UTIL_STACKTRACE ();
          t_  =  t; 
        }

        T get (void) 
        {
          return t_; 
        }

        void dump (std::string msg = "")
        {
          LOGSTR (DEBUG, "result_t_detail dump") 
            << this << ") : " 
            << saga::util::demangle (typeid (*this).name ()) 
            << " : " << t_ << std::endl;
        }

    };

  } // namespace impl

} // namespace saga

#endif // SAGA_ENGINE_RESULTTYPES_HPP

