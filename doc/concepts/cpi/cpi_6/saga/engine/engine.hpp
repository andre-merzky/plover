
#ifndef SAGA_ENGINE_HPP
#define SAGA_ENGINE_HPP

#include <map>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <saga/util/mutex.hpp>
#include <saga/util/scoped_lock.hpp>
#include <saga/util/shareable.hpp>
#include <saga/util/enums.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/adaptor/test/filesystem_adaptor_0.hpp>
#include <saga/adaptor/test/filesystem_adaptor_1.hpp>
#include <saga/adaptor/test/task_adaptor_0.hpp>

namespace saga
{
  namespace impl
  {
    // forward decl
    class cpi_base;
    class call_context;
    class engine;

    // make handling void call signatures in templates easier
    class void_t 
    {
      public:
        friend std::ostream & operator<< (std::ostream & o, void_t const & void_t);
    };
  
    std::ostream & operator<< (std::ostream & o, void_t const & v)
    {
      return o << "[void_t]";
    }

    //////////////////////////////////////////////////////////////////
    //
    // the result type thingies allow to keep get_result template free on impl
    // level and below - the template free base class is used wherever possible, 
    // and the types result_t_detail_ is only aquired as needed.
    //
    class result_t : public saga::util::shareable
    {
      public:
        virtual ~result_t (void) 
        {
          SAGA_UTIL_STACKTRACE ();
        }

        virtual void dump (std::string msg = "") 
        { 
          LOGSTR (DEBUG, "dump, result_t") << msg << " - (base result type)" << std::endl;
        }
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


    enum cpi_mode {
      Any        = 0, 
      Bound      = 1, 
      Simple     = 2, 
      Collective = 3, 
      Filter     = 4,
      All        = 5
    };

    SAGA_UTIL_REGISTER_ENUM (cpi_mode, Any       , 0);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode, Bounde    , 1);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode, Simple    , 2);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode, Collective, 3);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode, Filter    , 4);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode, All       , 5);


    enum call_mode  // FIXME: eventually becomes saga::task::mode
    { 
      Sync  = 0,
      Async = 1,
      Task  = 2
    };

    SAGA_UTIL_REGISTER_ENUM (call_mode, Sync , 0);
    SAGA_UTIL_REGISTER_ENUM (call_mode, Async, 1);
    SAGA_UTIL_REGISTER_ENUM (call_mode, Task , 2);


    enum call_state { // FIXME: eventually becomes saga::task::state
      Unknown    = 0,
      New        = 1,
      Running    = 2,
      Done       = 3,
      Failed     = 4,
      Canceled   = 5
    };

    SAGA_UTIL_REGISTER_ENUM (call_state, Unknown , 0);
    SAGA_UTIL_REGISTER_ENUM (call_state, New     , 1);
    SAGA_UTIL_REGISTER_ENUM (call_state, Running , 2);
    SAGA_UTIL_REGISTER_ENUM (call_state, Done    , 3);
    SAGA_UTIL_REGISTER_ENUM (call_state, Failed  , 4);
    SAGA_UTIL_REGISTER_ENUM (call_state, Canceled, 5);


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
        functor_base (std::string name)
          : name_ (name)
        {
          SAGA_UTIL_STACKTRACE ();
        }

        virtual ~functor_base (void) 
        {
          SAGA_UTIL_STACKTRACE ();
        }

        template <typename RET> 
        void init_functor_base (void)
        {
          SAGA_UTIL_STACKTRACE ();
          result_ = saga::util::shared_ptr <saga::impl::result_t_detail_ <RET> > (new saga::impl::result_t_detail_ <RET> ());
        }

        saga::util::shared_ptr <saga::impl::result_t> get_result  (void) 
        {
          return result_;
        }

        template <typename RET>
        saga::util::shared_ptr <saga::impl::result_t_detail_ <RET> > get_result  (void) 
        {
          SAGA_UTIL_STACKTRACE ();
          return result_.get_shared_ptr <saga::impl::result_t_detail_ <RET> > ();
        }

        std::string get_name (void) 
        {
          return name_;  
        }

        virtual void call_cpi (saga::util::shared_ptr <saga::impl::cpi_base>      cpi, 
                               saga::util::shared_ptr <saga::impl::call_context>  cc) = 0;
        // {
        //   SAGA_UTIL_STACKTRACE ();
        // };

        virtual void dump (std::string msg = "")
        {
          LOGSTR (DEBUG, "functor_base dump")
            << " (" << this << ") : " 
            << saga::util::demangle (typeid (*this).name ()) 
            << std::endl;
        }

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

    //////////////////////////////////////////////////////////////////
    //
    // container for things to pass around on any method call
    //
    class call_context : public saga::util::shareable
    {
      private:
        saga::util::shared_ptr <shareable>    impl_;            // calling object (has session)
        saga::impl::cpi_mode                  cpi_mode_;        // collect, simple, ...
        saga::impl::call_mode                 mode_;            // sync, async, task
        saga::impl::call_state                call_state_;      // new, running, done, failed ...
        saga::impl::call_state                task_state_;      // new, running, done, failed ...
    //  saga::exception                       exception_;       // exception stack collected from adaptors_used_/failed
    //  saga::util::timestamp                 created_;         // created time stamp
    //  saga::util::timestamp                 start_;           // start time stamp
    //  saga::util::timestamp                 stop_;            // stop time stamp
    //  saga::util::timestamp                 duration_;        // time needed for completion
        std::string                           func_name_;       // name of method which created functor
        std::string                           func_args_;       // for logging, args given to functor
    //  std::vector <saga::util::log::entry>  log_;             // some audit log
        std::vector <std::string>             adaptors_used_;   // adaptors which have been used (audit trail)
        std::vector <std::string>             adaptors_;        // adaptors to use
        std::vector <std::string>             adaptors_skip_;   // adaptors not to use

        saga::util::shared_ptr <result_t>     result_;          // store the result of an eventual async call
        saga::util::shared_ptr <functor_base> func_;            // the functor to call

      public:
        call_context (saga::util::shared_ptr <functor_base> func, 
                      saga::util::shared_ptr <shareable>    impl)
          : func_        (func)
          , impl_        (impl)
          , cpi_mode_    (Simple)
          , mode_        (Sync)
          , call_state_  (New)
          , task_state_  (New)
          , result_      ((new saga::impl::result_t_detail_ <saga::impl::void_t> ()))
        {
          SAGA_UTIL_STACKTRACE ();
        }

        saga::util::shared_ptr <functor_base> get_func (void)            { return func_;      } 
        saga::util::shared_ptr <shareable>    get_impl (void)            { return impl_;      } 

        void                   set_call_state (saga::impl::call_state s) { call_state_ = s;   }
        saga::impl::call_state get_call_state (void                    ) { return call_state_;}

        void                   set_task_state (saga::impl::call_state s) { task_state_ = s;   }
        saga::impl::call_state get_task_state (void)                     { return task_state_;}

        void                   set_mode       (saga::impl::call_mode  m) { mode_  = m;        }
        saga::impl::call_mode  get_mode       (void                    ) { return mode_;      }


        template <typename T>
        void init_result (void)
        {
          result_ = saga::util::shared_ptr <saga::impl::result_t_detail_ <T> > (new saga::impl::result_t_detail_ <T> ());
        }

        template <typename T>
        void set_result (T res)
        {
          result_.get_shared_ptr <saga::impl::result_t_detail_ <T> > ()->set (res);
        }

        template <typename T>
        T get_result (void)
        {
          return result_.get_shared_ptr <saga::impl::result_t_detail_ <T> > ()->get ();
        }

        saga::util::shared_ptr <saga::impl::result_t> get_result (void)
        {
          return result_;
        }

        void dump (std::string msg = "")
        {
          // FIXME: to_key can throw
          LOGSTR (DEBUG, "call_context dump")
            <<   "call_context (" << this << ") : " << msg << std::endl
            <<   "    cpi  mode   : " << saga::util::saga_enum_to_key <saga::impl::cpi_mode>   (cpi_mode_  ) << std::endl
            <<   "    call mode   : " << saga::util::saga_enum_to_key <saga::impl::call_mode>  (mode_      ) << std::endl
            <<   "    call state  : " << saga::util::saga_enum_to_key <saga::impl::call_state> (call_state_) << std::endl
            <<   "    task state  : " << saga::util::saga_enum_to_key <saga::impl::call_state> (task_state_) << std::endl
            <<   "    func name   : " << func_name_ << std::endl
            <<   "    func args   : " << func_args_ << std::endl;
          impl_.dump         ("    IMPL_       : ");
          impl_->dump        ();
          result_.dump       ("    result_: ");
          result_->dump      ();
          func_.dump         ("    func_       : ");
          func_->dump        ();
        }
    };

    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all IMPLs
    //
    class impl_base : public saga::util::shareable
    {
      protected:
        saga::util::shared_ptr <saga::impl::engine> engine_;

      public:
        impl_base (void);

        virtual ~impl_base (void) {}

        void dump (std::string msg = "");
        
        saga::util::shared_ptr <saga::impl::engine> get_engine (void) 
        { 
          return engine_; 
        }
    };

    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all CPIs
    //
    class cpi_base : public saga::util::shareable
    {
      private:
        std::map <std::string, void*> adata_;  // FIXME: check and use

      public:
        virtual ~cpi_base (void) {}

        virtual void dump (std::string msg = "")
        {
          LOGSTR (DEBUG, "cpi_base dump")
            << "cpi_base (" << this << ") : " 
            << saga::util::demangle (typeid (*this).name ()) << " : " 
            << msg << std::endl;
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

          ((*casted).*(call_)) (cc); 
        }

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

        void call_cpi (saga::util::shared_ptr <cpi_base>                 cpi, 
                       saga::util::shared_ptr <saga::impl::call_context> cc)
        { 
          SAGA_UTIL_STACKTRACE ();
          
          // cast cpi_base to CPI
          saga::util::shared_ptr <CPI> casted = cpi.get_shared_ptr <CPI> ();

          ((*casted).*(call_)) (cc, arg_1_); 
        }

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

    //////////////////////////////////////////////////////////////////////
    //
    // the SAGA Engine implementation, which loads all adaptors, and 
    // supports the SAGA file implementation.
    //
    // In reality, adaptor registration will be more complex:  if an adaptor
    // registers, the engine will inspect it, to register it for all CPI's it
    // knows about:
    //
    //   if ( adaptor.is_a <saga_object> () )
    //     object_cpis.push_back (adaptor);
    //   if ( adaptor.is_a <saga_attribute> () )
    //     attribute_cpis.push_back (adaptor);
    //   ...
    //
    // or something slightly more clever ;-)
    //
    class engine : public saga::util::shareable
    {
      private:
        std::vector <saga::util::shared_ptr <saga::impl::cpi_base> > cpis_;

      public:
        engine (void)
        {
          SAGA_UTIL_STACKTRACE ();

          LOGSTR (INFO, "engine") << "engine: register all adaptors";

          // create and register adaptor instances
          cpis_.push_back (open_adaptor <saga::adaptor::test::file_adaptor_0> ());
          cpis_.push_back (open_adaptor <saga::adaptor::test::file_adaptor_1> ());
          cpis_.push_back (open_adaptor <saga::adaptor::test::dir_adaptor_0>  ());
          cpis_.push_back (open_adaptor <saga::adaptor::test::dir_adaptor_1>  ());
          cpis_.push_back (open_adaptor <saga::adaptor::test::task_adaptor_0> ());
        }

        template <typename ADP>
        saga::util::shared_ptr <saga::impl::cpi_base> open_adaptor (void)
        {
          SAGA_UTIL_STACKTRACE ();
          return saga::util::shared_ptr <saga::impl::cpi_base> (new ADP);
        }


        // get_cpi's is used by the API class implementation to get an (ordered)
        // list of adaptors to call
        template <typename CPI>
        std::vector <saga::util::shared_ptr <CPI> > get_cpis (void)
        {
          std::vector <saga::util::shared_ptr <CPI> > ret;

          for ( unsigned int i = 0; i < cpis_.size (); i++ )
          {
            if ( cpis_[i].is_a <CPI> () )
            {
              ret.push_back (cpis_[i].get_shared_ptr <CPI> ());
            }
          }

          return ret;
        }

        //////////////////////////////////////////////////////////////////
        //
        // 
        //
        template <typename IMPL, typename CPI>
        void call (saga::util::shared_ptr <saga::impl::call_context> cc)
        {
          SAGA_UTIL_STACKTRACE ();

          typedef saga::impl::functor_base             func_base_t;
          // typedef saga::impl::functor <IMPL, CPI, RET> func_cast_t;

          // get the matching list of CPIs
          std::vector <saga::util::shared_ptr <CPI> > cpis = get_cpis <CPI> ();

          // try one adaptor after the other, until one succeeds.
          LOGSTR (INFO, "engine call") << "calling cpis " << cpis.size () << std::endl;
          for ( unsigned int i = 0; i < cpis.size (); i++ )
          {
            LOGSTR (INFO, "engine call") << "calling cpi " << i << " / " << cpis.size () << std::endl;
            try
            {
              LOGSTR (INFO, "engine call") << "adaptor " << i << " : calling " << cc->get_func()->get_name () << std::endl;

              saga::util::shared_ptr <func_base_t> base   = cc->get_func ();
              // saga::util::shared_ptr <func_cast_t> casted = base.get_shared_ptr <func_cast_t> ();

              base->call_cpi (cpis[i], cc);
              cc->set_call_state (Done);
              LOGSTR (INFO, "engine call") << "adaptor " << i << " : succeeded for " << cc->get_func()->get_name () << std::endl;

              return;
            }
            catch ( const char * m )
            {
              // something went wrong...
              LOGSTR (INFO, "engine call") << "adaptor " << i << " : failed for " << cc->get_func()->get_name () << " : " << m << std::endl;
            }
            catch ( ... )
            {
              // something went wrong...
              LOGSTR (INFO, "engine call") << "adaptor " << i << " : failed for " << cc->get_func()->get_name () << " : ???" << std::endl;
            }
            LOGSTR (INFO, "engine call") << "calling cpi done " << i << " / " << cpis.size () << std::endl;
          }

          // no adaptor suceeded.  We don't have anything sensible to return, so
          // we flag the failure, and throw.  That is redundant, but hey...
          cc->set_call_state (Failed);

          LOGSTR (INFO, "engine call") << "all adaptors failed for " << cc->get_func()->get_name () << std::endl;
          throw "no adaptor suceeded";
        }

        void dump (std::string msg = "")
        {
          LOGSTR (DEBUG, "engine dump") 
            << "engine (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;

          for ( unsigned int i = 0; i < cpis_.size (); i++ )
          {
            cpis_[i].dump();
          }
        }
    };

  } // namespace impl

} // namespace saga

#endif //  SAGA_ENGINE_HPP

