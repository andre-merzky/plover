
#ifndef SAGA_ENGINE_FUNCTORS_HPP
#define SAGA_ENGINE_FUNCTORS_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/engine/result_types.hpp>

namespace saga
{
  namespace impl
  {
    // forward decl
    class cpi_base;
    class call_context;

    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all functors
    //
    class functor_base : public saga::util::shareable
    {
      private:
        std::string name_;                                     // name of function call 
        saga::util::shared_ptr <saga::impl::result_t> result_; // container for function call result

      public: 
        functor_base (std::string name);
        virtual ~functor_base (void);

        template <typename RET> 
        void init_functor_base (void)
        {
          SAGA_UTIL_STACKTRACE ();
          result_ = saga::util::shared_ptr <saga::impl::result_t_detail_ <RET> > (new saga::impl::result_t_detail_ <RET> ());
        }


        template <typename T>
        void set_result (T res)
        {
          SAGA_UTIL_STACKTRACE ();

          // FIXME: confirm correct type
          result_.get_shared_ptr <saga::impl::result_t_detail_ <T> > ()->set (res);
        }

        template <typename T>
        bool has_result_type  (void) 
        {
          SAGA_UTIL_STACKTRACE ();

          // FIXME: confirm correct type
          return result_.is_a <saga::impl::result_t_detail_ <T> > ();
        }


        template <typename T>
        T get_result  (void) 
        {
          SAGA_UTIL_STACKTRACE ();
          // FIXME: confirm correct type
          if ( ! has_result_type <T> () )
          {
            throw "Incorrect result type requested";
          }

          return result_.get_shared_ptr <saga::impl::result_t_detail_ <T> > ()->get ();
        }

        saga::util::shared_ptr <saga::impl::result_t> get_result  (void);
        std::string                                   get_name (void);

        virtual int  nargs (void) { return -1; }

        virtual void call_cpi (saga::util::shared_ptr <saga::impl::cpi_base>      cpi, 
                               saga::util::shared_ptr <saga::impl::call_context>  cc) = 0;

        virtual void dump (std::string msg = "");

    };

    template <typename IMPL, typename CPI, typename RET>
    class functor : public functor_base
    {
      public:
        functor (std::string name)
          : functor_base (name)
        {
          SAGA_UTIL_STACKTRACE ();
          this->init_functor_base <RET> ();
        }

        virtual ~functor (void) 
        {
          SAGA_UTIL_STACKTRACE ();
        }

        virtual void dump (std::string msg = "")
        {
          LOGSTR (DEBUG, "functor dump")
            << "functor         : " << this << " : " << saga::util::demangle (typeid (*this).name ()) << std::endl
            << "    IMPL        : " << saga::util::demangle (typeid (IMPL ).name ()) << std::endl
            << "    CPI         : " << saga::util::demangle (typeid (CPI  ).name ()) << std::endl
            << "    RET         : " << saga::util::demangle (typeid (RET  ).name ()) << std::endl;
          get_result ().dump ("    result_     : ");
          get_result ()->dump();
        }
    };


    /////////////////////////////////////////////////////////////////
    //
    // functor: stores a cpi function pointer and call arguments, and 
    // calls it on a given cpi
    //
    //////////////////////////////////////////////////////////////////
    // functor with 0 args
    template <typename IMPL, 
              typename CPI, 
              typename RET>
    class functor_0 : public functor <IMPL, CPI, RET>
    {
      private:           
        void (CPI::* call_)(saga::util::shared_ptr <call_context>);

      public: 
        functor_0 (std::string name, 
                   void (CPI::*call )(saga::util::shared_ptr <call_context>))
          : functor <IMPL, CPI, RET> (name)
          , call_   (call) 
        { 
          SAGA_UTIL_STACKTRACE ();
        }

        void call_cpi (saga::util::shared_ptr <cpi_base>                 cpi, 
                       saga::util::shared_ptr <saga::impl::call_context> cc)
        { 
          SAGA_UTIL_STACKTRACE ();

          // cast cpi_base to CPI
          saga::util::shared_ptr <CPI> casted = cpi.get_shared_ptr <CPI> ();

          // FIXME: assert valid function ptr
          ((*casted).*(call_)) (cc); 
        }

        virtual int  nargs (void) { return 0; }

        void dump (std::string msg = "")
        {
          LOGSTR (DEBUG, "functor_0 dump") 
            << "functor (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl
            << "    call        : " << saga::util::demangle (typeid (call_).name ()) << " : " << this->get_name () << std::endl
            << "    IMPL        : " << saga::util::demangle (typeid (IMPL ).name ()) << std::endl
            << "    CPI         : " << saga::util::demangle (typeid (CPI  ).name ()) << std::endl
            << "    RET         : " << saga::util::demangle (typeid (RET  ).name ()) << std::endl;
          this->get_result ().dump ("    result_     : ");
          this->get_result ()->dump();
        }
    };

    //////////////////////////////////////////////////////////////////
    // functor with 1 arg
    template <typename IMPL, 
              typename CPI, 
              typename RET, 
              typename ARG_1> 
    class functor_1 : public functor <IMPL, CPI, RET>
    {
      private:           
        void (CPI::* call_)(saga::util::shared_ptr <call_context>, ARG_1); 
        ARG_1        arg_1_;


      public: 
        functor_1 (std::string name, 
                   void (CPI::*call )(saga::util::shared_ptr <call_context>, ARG_1), 
                   ARG_1 arg_1) 
          : functor <IMPL, CPI, RET> (name)
          , call_   (call) 
          , arg_1_  (arg_1) 
        { 
          SAGA_UTIL_STACKTRACE ();
        }

        void set_arg_1 (ARG_1 a1)
        {
          arg_1_ = a1;
        }

        template <typename T>
        bool has_arg_1_type (void)
        {
          return ( typeid (ARG_1) == typeid (T) );
        }

        ARG_1 get_arg_1 (void)
        {
          if ( ! has_arg_1_type <T> )
          {
            throw "requested incorrect arg_1 type";
          }

          return arg_1_;
        }

        void call_cpi (saga::util::shared_ptr <cpi_base>                 cpi, 
                       saga::util::shared_ptr <saga::impl::call_context> cc)
        { 
          SAGA_UTIL_STACKTRACE ();
          
          // cast cpi_base to CPI
          if ( ! cpi.is_a <CPI> () )
          {
            throw "Cannot handle CPI type in call_cpi";
          }

          saga::util::shared_ptr <CPI> casted = cpi.get_shared_ptr <CPI> ();

          // ::assert (NULL != call_);
          // ::assert (NULL != casted);

          ((*casted).*(call_)) (cc, arg_1_); 
        }

        virtual int  nargs (void) { return 1; }

        void dump (std::string msg = "")
        {
          LOGSTR (DEBUG, "functor_1 dump") 
            << "functor (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl
            << "    call        : " << saga::util::demangle (typeid (call_).name ()) << " : " << this->get_name () << std::endl
            << "    ARG 1       : " << saga::util::demangle (typeid (ARG_1).name ()) << " : " << arg_1_      << std::endl
            << "    IMPL        : " << saga::util::demangle (typeid (IMPL ).name ()) << std::endl
            << "    CPI         : " << saga::util::demangle (typeid (CPI  ).name ()) << std::endl
            << "    RET         : " << saga::util::demangle (typeid (RET  ).name ()) << std::endl;
          this->get_result ().dump ("    result_     : ");
          this->get_result ()->dump();
        }
    };

    //////////////////////////////////////////////////////////////////
    //
    // FIXME: functor versions for more args follow here....
    //


  } // namespace impl

} // namespace saga

#endif //  SAGA_ENGINE_FUNCTORS_HPP

