
#ifndef SAGA_ENGINE_FUNC_HPP
#define SAGA_ENGINE_FUNC_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

// FIXME: we do not actiually need RET, but is kind of keeps the whole function
// signature together in one place.  Kind of...

namespace saga
{
  namespace impl
  {
    // forward decl
    class cpi_base;
    class call_context;

    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all funcs
    //
    class func_base : public saga::util::shareable
    {
      private:
        std::string name_;    // name of function call 

      public: 
        func_base (std::string name);
        virtual ~func_base (void);

        std::string                                   get_name (void);

        virtual int  nargs (void) { return -1; }

        virtual void call_cpi (saga::util::shared_ptr <saga::impl::cpi_base>      cpi, 
                               saga::util::shared_ptr <saga::impl::call_context>  cc) = 0;

        virtual void dump (std::string msg = "");

    };

    template <typename IMPL, typename CPI, typename RET>
    class func : public func_base
    {
      public:
        func (std::string name)
          : func_base (name)
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

        virtual int  nargs (void) { return 0; }

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

        virtual int  nargs (void) { return 1; }

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

