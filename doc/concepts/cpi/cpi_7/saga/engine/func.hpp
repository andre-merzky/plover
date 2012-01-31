
#ifndef SAGA_ENGINE_FUNC_HPP
#define SAGA_ENGINE_FUNC_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/engine/result_types.hpp>

// FIXME: do we need to support functions with more than one return value?

namespace saga
{
  namespace impl
  {
    // forward decl
    class cpi_base;
    class call_context;

    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all funcs, no templates
    //
    class func_base : public saga::util::shareable
    {
      protected:
        std::string                        name_;    // name of function call 
        saga::util::shared_ptr <result_t>  result_;  // call result container

        func_base (std::string name, result_t * rp);

      public: 
        virtual ~func_base (void);


        virtual void call_cpi (saga::util::shared_ptr <saga::impl::cpi_base>      cpi, 
                               saga::util::shared_ptr <saga::impl::call_context>  cc) = 0;


        std::string get_name  (void);

        virtual void dump (std::string msg = "");

        template <typename T>
        void set_result (T res)
        {
          // FIXME: make sure this is called only once, or, at least, always
          // called with the same type.  We might want to do that on
          // result_t_detail level though.  Also, this ambiguity would be
          // avoided by class level templatization

          SAGA_UTIL_STACKTRACE ();

          LOGSTR (DEBUG, "call_context set_result") << "typeset   " << saga::util::demangle (typeid (T).name ()) << std::endl;

          result_->set <T> (res);
        }

        template <typename T>
        bool has_result_type  (void) 
        {
          SAGA_UTIL_STACKTRACE ();

          if ( ! result_ )
          {
            // no type set, yet
            SAGA_UTIL_STACKDUMP ();
            throw "result type is not yet defined";
          }

          return result_->has_a <T> ();
        }


        template <typename T>
        T get_result  (void) 
        {
          SAGA_UTIL_STACKTRACE ();

          if ( ! result_ )
          {
            // no type set, yet
            SAGA_UTIL_STACKDUMP ();
            throw "result type is not yet set";
          }

          if ( ! has_result_type <T> () )
          {
            LOGSTR (DEBUG, "call_context get_result") << "requested " << saga::util::demangle (typeid (T).name ()) << std::endl;
            LOGSTR (DEBUG, "call_context get_result") << "available " << result_->get_ptype () << std::endl;

            SAGA_UTIL_STACKDUMP ();
            throw "Incorrect result type requested";
          }

          return result_->get <T> ();
        }

        saga::util::shared_ptr <result_t> get_result  (void)
        {
          return result_;
        }
    };

    template <typename IMPL, typename CPI, typename RET>
    class func : public func_base
    {
      private:
        // we only want to have the result accessed via func_base.  
        // FIXME: check if the funcs below would simplify code elsewhere.
        void set_result      (RET ret);
        RET  get_result      (void)   ;
        bool has_result_type (void)   ;

      public:
        func (std::string name)
          : func_base (name, new saga::impl::result_t_detail_ <RET> ())
        {
          SAGA_UTIL_STACKTRACE ();
        }

        virtual ~func (void) 
        {
          SAGA_UTIL_STACKTRACE ();
        }

        virtual void dump (std::string msg = "")
        {
          LOGSTR (DEBUG, "func dump")
            << "func         : " << this << " : " << saga::util::demangle (typeid (*this).name ()) << std::endl
            << "    IMPL        : " << saga::util::demangle (typeid (IMPL ).name ()) << std::endl
            << "    CPI         : " << saga::util::demangle (typeid (CPI  ).name ()) << std::endl
            << "    RET         : " << saga::util::demangle (typeid (RET  ).name ()) << std::endl;
          result_.dump       ("    result_     : ");
          if ( result_ )
            result_->dump    ();
        }
    };


    /////////////////////////////////////////////////////////////////
    //
    // func: stores a cpi function pointer and call arguments, and 
    // calls it on a given cpi
    //
    //////////////////////////////////////////////////////////////////
    // func with 0 args
    template <typename IMPL, 
              typename CPI, 
              typename RET>
    class func_0 : public func <IMPL, CPI, RET>
    {
      private:           
        void (CPI::* call_)(saga::util::shared_ptr <call_context>);

      public: 
        func_0 (std::string name, 
                   void (CPI::*call )(saga::util::shared_ptr <call_context>))
          : func <IMPL, CPI, RET> (name)
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

        void dump (std::string msg = "")
        {
          LOGSTR (DEBUG, "func_0 dump") 
            << "func (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl
            << "    call        : " << saga::util::demangle (typeid (call_).name ()) << " : " << this->get_name () << std::endl
            << "    IMPL        : " << saga::util::demangle (typeid (IMPL ).name ()) << std::endl
            << "    CPI         : " << saga::util::demangle (typeid (CPI  ).name ()) << std::endl
            << "    RET         : " << saga::util::demangle (typeid (RET  ).name ()) << std::endl;
        }
    };

    //////////////////////////////////////////////////////////////////
    // func with 1 arg
    template <typename IMPL, 
              typename CPI, 
              typename RET, 
              typename ARG_1> 
    class func_1 : public func <IMPL, CPI, RET>
    {
      private:           
        void (CPI::* call_)(saga::util::shared_ptr <call_context>, ARG_1); 
        ARG_1        arg_1_;


      public: 
        func_1 (std::string name, 
                   void (CPI::*call )(saga::util::shared_ptr <call_context>, ARG_1), 
                   ARG_1 arg_1) 
          : func <IMPL, CPI, RET> (name)
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
          if ( ! has_arg_1_type <ARG_1> )
          {
            SAGA_UTIL_STACKDUMP ();
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
            SAGA_UTIL_STACKDUMP ();
            throw "Cannot handle CPI type in call_cpi";
          }

          saga::util::shared_ptr <CPI> casted = cpi.get_shared_ptr <CPI> ();

          // ::assert (NULL != call_);
          // ::assert (NULL != casted);

          ((*casted).*(call_)) (cc, arg_1_); 
        }

        void dump (std::string msg = "")
        {
          LOGSTR (DEBUG, "func_1 dump") 
            << "func (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl
            << "    call        : " << saga::util::demangle (typeid (call_).name ()) << " : " << this->get_name () << std::endl
            << "    ARG 1       : " << saga::util::demangle (typeid (ARG_1).name ()) << " : " << arg_1_      << std::endl
            << "    IMPL        : " << saga::util::demangle (typeid (IMPL ).name ()) << std::endl
            << "    CPI         : " << saga::util::demangle (typeid (CPI  ).name ()) << std::endl
            << "    RET         : " << saga::util::demangle (typeid (RET  ).name ()) << std::endl;
        }
    };

    //////////////////////////////////////////////////////////////////
    //
    // FIXME: func versions for more args follow here....
    //


  } // namespace impl

} // namespace saga

#endif //  SAGA_ENGINE_FUNC_HPP

