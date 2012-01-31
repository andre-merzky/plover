
#ifndef SAGA_ENGINE_RESULTTYPES_HPP
#define SAGA_ENGINE_RESULTTYPES_HPP

#include <iostream>
#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

//////////////////////////////////////////////////////////////////////
// 
// a call context represents a function call to be dispatched to a 
// backend.  That function call usually has a typed return value.
// In order to keep the call_context template free, we wrap that
// result in a separate result_t.  Well, result_t itself is a 
// interface class used by the cc -- the actual templetized container
// inherits that interface class.
//
// class call_context
// {
//   private:
//    shared_ptr <result_t> result_; // no template here
//
//  public:
//    template <typename T>
//    void set_result (T res)
//    {
//        result_ = shared_ptr <result_t> (new result_t_detail_ <T> ());
//        result_->set <T> (res);
//    }
//
//    template <typename T>
//    T get_result (void) 
//    {
//      return result_->get <T> ();
//    }
// }
//
//////////////////////////////////////////////////////////////////////

namespace saga
{
  namespace impl
  {
    //////////////////////////////////////////////////////////////////
    //
    // make handling void call signatures in templates easier
    //
    class void_t 
    {
      public:
        friend std::ostream & operator<< (std::ostream & o, void_t const & void_t);
    };

    std::ostream & operator<< (std::ostream & o, void_t const & v);


    //////////////////////////////////////////////////////////////////
    //
    // foward declaration of result_t_detail_
    template <typename T> 
    class result_t_detail_;

    //////////////////////////////////////////////////////////////////
    //
    // the result type classes allow to keep call_context template-free on impl
    // level and below - the template-free result_t base class is used wherever
    // possible, and the typed result_t_detail_ is only aquired as needed.
    //
    class result_t : public saga::util::shareable
    {
      private:
        // The c'tor is private to avoid instancies of this
        // class to get created, only its friend, the inheriting 
        // result_t_detail_<T>, can axxess the c'tor.
        //
        // FWIW, 'protected' would have worked as well, and avoid the friend,
        // but with 'private' we basically allow *only* result_t_detail to inherit 
        // from us.

        template <class T>
        friend class result_t_detail_;

        result_t (void) 
        {
          SAGA_UTIL_STACKTRACE ();
        }


      public:
        virtual ~result_t (void)
        {
          SAGA_UTIL_STACKTRACE ();
        }

        // check if the result_detail type is correct
        template <typename T>
        bool has_a (void)
        {
          saga::util::shared_ptr <result_t> shared_me = shared_this <result_t> (); // this should always work... (tm)

          // make shure that 'this' is pointing to a result_t_detail of the
          // requested type
          if ( shared_me.is_a <result_t_detail_ <T> > () )
          {
            return true;
          }

          return false;
        }


        // FIXME: get() and set() can throw at the moment.  That needs to be
        // well documented to the adaptor developer, as 
        //
        //   cc->set_result <int>   (300);
        //   cc->set_result <float> (3.0);
        //
        // will throw.  I don't see a way around this though, as in the above
        // example, there does not seem to exist a sensible recovery strategy...

        // get result from container
        template <typename T>
        T get (void)
        {
          saga::util::shared_ptr <result_t> shared_me = shared_this <result_t> (); // this should always work... (tm)

          // make shure that 'this' is pointing to a result_t_detail of the
          // requested type
          if ( ! has_a <T> () )
          {
            throw "get() on invalid result type";
          }

          return shared_this <result_t_detail_ <T> > ()->get ();
        }

        // set result in container
        template <typename T>
        void set (T res)
        {
          saga::util::shared_ptr <result_t> shared_me = shared_this <result_t> (); // this should always work... (tm)

          // make shure that 'this' is pointing to a result_t_detail of the
          // requested type
          if ( ! has_a <T> () )
          {
            throw "set() on invalid result type";
          }

          shared_this <result_t_detail_ <T> > ()->set (res);
        }


        // forward the dump call to any inheriting class,
        virtual void dump (std::string msg = "") = 0;
    };


    //////////////////////////////////////////////////////////////////
    //
    // This is a templatized container for func return values -- whenever
    // possible, its template-free result_t base class is used and passed
    // around -- only the actual value set/get methods require the
    // templatization.
    //
    // all members of this class are private, so that it cannot be used
    // directly, but only via its result_t interface.
    //
    template <typename T>
    class result_t_detail_ : public result_t
    {
      // this class should only be used via its result_t interface - all 
      // methods are private otherwise.
      friend class result_t;

      private:
        T t_;

        // we should not need any specific c'tor or copy / assignment ops --
        // instead, we rely on T having those.


        void set (T t) 
        {
          SAGA_UTIL_STACKTRACE ();
          t_  =  t; 
        }

        T get (void) 
        {
          SAGA_UTIL_STACKTRACE ();
          return t_; 
        }

        // NOTE: dump assumes that T can be printed to an ostream...
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

