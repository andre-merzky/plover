
#ifndef SAGA_CPI_4_HPP
#define SAGA_CPI_4_HPP

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
#include <saga/util/stack_tracer.hpp>

// In the cpi_4 example, we introduces a functor class, which allows the
// engine to more indirectly mediate between API and adaptor, and also 
// added the ability to pass API instance data from the API to the adaptor.
// Those instance data represent the API object state, as maintained over 
// successive adaptor invocations. 
//
// The presented scheme now forms an almost complete SAGA stack, but a 
// couple of features are missing:
//
//   - different cpi-call modes
//   - asynchronous operations
//   - adaptor state management
//   - adaptor suite state management
//   - callbacks and metrics
//   - bulk operations (i.e.., clever abuse of task containers)
//   - filter-adaptors (which call other adaptors)
//  
// This iteration will implement 
//
//   cpi-call modes
//     some calls may require to collect results over multiple
//     adaptors (js.list_jobs()), others may use adaptors for 
//     just filtering, calling one adaptor after the other 
//     (task_container).  
//
//   filter-adaptors
//     adaptors may do something to the call's state and instance
//     data, and then call into the engine again.  An example might
//     be task_container, or basic parameter and state checks
//
//  For those features, it seems useful to extent the concept of Functors, and
//  to attach additional information and state to it, such as
//
//   - adaptors to use
//   - adaptors not to use
//   - cpi-call mode
//   - async mode
//   - exception stack
//   - task state
//   - timing info
//   - inspection info (name, calling object id, args?)
//
//  We find, however, that functors become rather complex that way, so we 
//  split it in the functor proper (holding the cpi function pointer to call)
//  and a calling context (cc) which holds the functor and the remaining 
//  info, and is passed from impl via the engine to the adaptors and back.  
//  To a large extent, a CC internally represents a saga::task.
//
//  To reduce the templetism of the code, only the functor itself is
//  templetized, and is stored as a shared pointer to an untempletized base
//  class.  The user of the functor have to cast it back, but can use inspection
//  before doing so.  That way, we keep the use of templates confined.
//
namespace saga
{
  //////////////////////////////////////////////////////////////////
  //
  // prototypes and declarations
  //
  // note that the structure here still differs from the actual code structure, in
  // order to keep this example within a single source file.
  //

  namespace impl
  {
    class cpi_base;

    class result_t;
    template <typename T>
    class result_t_detail_;

    class task;
    class task_cpi;
    class task_instance_data;

    namespace filesystem
    {
      class file;
      class file_cpi;
      class file_instance_data;

      class dir;
      class dir_cpi;
      class dir_instance_data;
    }

  }
  namespace adaptor
  {
    namespace test
    {
      class task_adaptor_0;

      class file_adaptor_0;
      class file_adaptor_1;

      class dir_adaptor_0;
      class dir_adaptor_1;
    }
  }
  namespace impl
  {
    class engine;
    class functor_base;
    class call_context;

    template <typename IMPL, typename CPI, typename RET> class functor;
    template <typename IMPL, typename CPI, typename RET>                 class functor_0;
    template <typename IMPL, typename CPI, typename RET, typename ARG_1> class functor_1;
    
  }

  class task;
  namespace filesystem
  {
    class file;
    class dir;
  }


  namespace impl
  {
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
          std::cout << msg << " - (base result type)" << std::endl; 
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
          std::cout << "result_t_detail_ (" << this << ") : " 
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
          std::cout << "functor_base (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << std::endl;
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
          std::cout  << "functor         : " << this << " : " << saga::util::demangle (typeid (*this).name ()) << std::endl;
          std::cout  << "    IMPL        : " << saga::util::demangle (typeid (IMPL ).name ()) << std::endl;
          std::cout  << "    CPI         : " << saga::util::demangle (typeid (CPI  ).name ()) << std::endl;
          std::cout  << "    RET         : " << saga::util::demangle (typeid (RET  ).name ()) << std::endl;
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
        std::vector <std::string>             adaptors_used_;   // adaptors which have been used (audit trail)
        std::vector <std::string>             adaptors_;        // adaptors to use
        std::vector <std::string>             adaptors_skip_;   // adaptors not to use
        saga::util::shared_ptr <shareable>    impl_;            // calling object (has session)
        saga::impl::cpi_mode                  cpi_mode_;        // collect, simple, ...
        saga::impl::call_mode                 mode_;            // sync, async, task
        saga::impl::call_state                state_;           // new, running, done, failed ...
        saga::impl::call_state                task_state_;      // new, running, done, failed ...
    //  saga::exception                       exception_;       // exception stack collected from adaptors_used_/failed
    //  saga::util::timestamp                 created_;         // created time stamp
    //  saga::util::timestamp                 start_;           // start time stamp
    //  saga::util::timestamp                 stop_;            // stop time stamp
    //  saga::util::timestamp                 duration_;        // time needed for completion
        std::string                           func_name_;       // name of method which created functor
        std::string                           func_args_;       // for logging, args given to functor
    //  std::vector <saga::util::log::entry>  log_;             // some audit log

        saga::util::shared_ptr <result_t>     result_;          // store the result of an eventual async call
        saga::util::shared_ptr <functor_base> func_;            // the functor to call

      public:
        call_context (saga::util::shared_ptr <functor_base> func, 
                      saga::util::shared_ptr <shareable>    impl)
          : func_        (func)
          , impl_        (impl)
          , cpi_mode_    (Simple)
          , mode_        (Sync)
          , state_       (New)
          , task_state_  (New)
          , result_      ((new saga::impl::result_t_detail_ <saga::impl::void_t> ()))
        {
          SAGA_UTIL_STACKTRACE ();
        }

        saga::util::shared_ptr <functor_base> get_func (void)            { return func_;      } 
        saga::util::shared_ptr <shareable>    get_impl (void)            { return impl_;      } 

        void                   set_state      (saga::impl::call_state s) { state_ = s;        }
        saga::impl::call_state get_state      (void                    ) { return state_;     }

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
          std::cout      <<   "call_context (" << this << ") : " << msg << std::endl;
          std::cout      <<   "    cpi  mode   : " << saga::util::saga_enums.to_key <saga::impl::cpi_mode>   (cpi_mode_  ) << std::endl;
          std::cout      <<   "    call mode   : " << saga::util::saga_enums.to_key <saga::impl::call_mode>  (mode_      ) << std::endl;
          std::cout      <<   "    call state  : " << saga::util::saga_enums.to_key <saga::impl::call_state> (state_     ) << std::endl;
          std::cout      <<   "    task state  : " << saga::util::saga_enums.to_key <saga::impl::call_state> (task_state_) << std::endl;
          std::cout      <<   "    func name   : " << func_name_ << std::endl;
          std::cout      <<   "    func args   : " << func_args_ << std::endl;
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
          std::cout << "cpi_base (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
        }
    };

    //////////////////////////////////////////////////////////////////
    //
    // IMPL namespaces and classes
    //
    //////////////////////////////////////////////////////////////////////
    //
    // our task adaptor base class
    //
    class task_cpi : public saga::impl::cpi_base
    {
      public:
        virtual ~task_cpi (void) { };

        virtual void constructor (saga::util::shared_ptr <call_context> cc)
        { 
          SAGA_UTIL_STACKTRACE ();
          throw "constructor: NotImplemented"; 
        } 

        virtual void get_state (saga::util::shared_ptr <call_context> cc)
        {
          SAGA_UTIL_STACKTRACE ();
          throw "get_state : NotImplemented"; 
        }

        // FIXME: cont here (any needed)
        virtual void get_result (saga::util::shared_ptr <call_context> cc)
        { 
          SAGA_UTIL_STACKTRACE ();
          throw "get_result : NotImplemented"; 
        } 
    };

    class task_instance_data : public saga::util::shareable
    {
      // saga::impl::task_instance_data manages the state information for one
      // specific saga::task instance.  Note that this state is shared by all
      // task adaptors: they can obtained a scoped-locked copy of it via
      // get_instance_data()
      private:
        // saga::session s_;

      public:
        saga::util::shared_ptr <saga::impl::call_context> t_cc;

        void dump (std::string msg = "")
        {
          std::cout << "task_instance_data (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
          std::cout << "    t_cc        : " << std::endl;
          t_cc.dump ();
          t_cc->dump ();  // should give infinite recursion!
        }
    };


    class task : public saga::impl::impl_base
    {
      private:
        saga::util::shared_ptr <task_instance_data> idata_;

      public:
        task (void);
        task (saga::util::shared_ptr <saga::impl::call_context> cc);

        void_t                            constructor (void);
        saga::impl::call_state            get_state   (void);
        saga::util::shared_ptr <result_t> get_result  (void);

        // allow adaptor to obtain instance data (unlocked)
        saga::util::shared_ptr <task_instance_data> get_instance_data (void)
        {
          return idata_;
        }

        virtual void dump (std::string msg = "")
        {
          std::cout << "impl::task (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
          idata_.dump ("    idata_      : ");
          idata_->dump();
        }

    }; // class task

    //////////////////////////////////////////////////////////////////
    //
    // the filesystem implementation
    //
    namespace filesystem
    {
      //////////////////////////////////////////////////////////////////////
      //
      // our file adaptor base class
      //
      class file_cpi : public saga::impl::cpi_base
      {
        public:
          virtual ~file_cpi (void) 
          {
            SAGA_UTIL_STACKTRACE ();
          };

          virtual void constructor (saga::util::shared_ptr <call_context> cc, 
                                      std::string                           url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            throw "constructor: NotImplemented"; 
          } 

          virtual void get_size (saga::util::shared_ptr <call_context> cc)
          { 
            SAGA_UTIL_STACKTRACE ();
            throw "get_size : NotImplemented"; 
          } 

          //  we have a second get_size method for the async versions.  The
          //  call_mode parameter is somewhat redundant, as the cm is also
          //  stored in the cc, but it allows to use overloading in the adaptor
          //  for the various sync/async calls.  The adaptor needs to switch
          //  over the enum to see what async flavor is wanted / needed, 
          //  but the returned task's state can easily be adjusted by the
          //  calling functor or by the engine.
          virtual void get_size (saga::util::shared_ptr <call_context> cc, 
                                 saga::impl::call_mode                 m)
          { 
            SAGA_UTIL_STACKTRACE ();
            throw "get_size <...> : NotImplemented"; 
          } 

          virtual void copy (saga::util::shared_ptr <call_context> cc, 
                             std::string                           tgt)
          {
            SAGA_UTIL_STACKTRACE ();
            throw "copy : NotImplemented"; 
          }
      };

      // saga::impl::file_instance_data manages the state information for one
      // specific saga::file instance.  Note that this state is shared by all
      // file adaptors: they can obtained a scoped-locked copy of it via
      // get_instance_data()
      class file_instance_data : public saga::util::shareable
      {
        private:
          // saga::session s_;

        public:
          std::string url;
          size_t      pos;
          bool        valid;

          void dump (std::string msg = "")
          {
            std::cout << "file_instance_data (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
            std::cout << "    valid       : " << valid << std::endl;
            std::cout << "    url         : " << url   << std::endl;
            std::cout << "    pos         : " << pos   << std::endl;
          }
      };


      // saga impl class
      class file : public saga::impl::impl_base
      {
        private:
          saga::util::shared_ptr <file_instance_data> idata_;

        public:
          file (void);

          // allow adaptor to obtain instance data (unlocked)
          saga::util::shared_ptr <file_instance_data> get_instance_data (void)
          {
            return idata_;
          }

          // as the constructor is also a cpi method, and we thus want to call the
          // cpi, we want to be finished with the actual object construction,
          // and the shared_ptr setup.  Thus impl construction is rendered as
          // a two-step process
          void_t constructor (std::string url);

          // instead of get_size calling each cpi individually, the call invocation
          // is passed off to the engine
          int get_size (void);

          // async version of same call
          saga::util::shared_ptr <saga::impl::task> get_size (saga::impl::call_mode);

          // other calls for copy
          void_t copy (std::string tgt);

          void dump (std::string msg = "")
          {
            std::cout << "impl::file (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
            idata_.dump  ("    idata_      : ");
            idata_->dump ();
            impl_base::dump ();
          }
      }; // class file

      ///////////////////////////////////////////////////////////////////////////////////////////

      class dir_cpi : public saga::impl::cpi_base
      {
        public:
          virtual ~dir_cpi (void) 
          {
            SAGA_UTIL_STACKTRACE ();
          };

          virtual void constructor (saga::util::shared_ptr <call_context> cc, 
                                      std::string                           url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            throw "constructor: NotImplemented"; 
          } 

          virtual void get_url (saga::util::shared_ptr <call_context> cc)
          { 
            SAGA_UTIL_STACKTRACE ();
            throw "get_url : NotImplemented"; 
          } 

          virtual void open (saga::util::shared_ptr <call_context> cc, 
                                                                              std::string                           url)
          {
            SAGA_UTIL_STACKTRACE ();
            throw "open : NotImplemented"; 
          }
      };

      // saga::impl::dir_instance_data manages the state information for one
      // specific saga::dir instance.  Note that this state is shared by all
      // dir adaptors: they can obtained a scoped-locked copy of it via
      // get_instance_data()
      class dir_instance_data : public saga::util::shareable
      {
        private:
          saga::util::mutex mtx_;
          // saga::session s_;

        public:
          bool        valid;
          std::string url;
          size_t      pos;

          void dump (std::string msg = "")
          {
            std::cout << "dir_instance_data (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
            std::cout << "    valid       : " << valid << std::endl;
            std::cout << "    url         : " << url   << std::endl;
            std::cout << "    pos         : " << pos   << std::endl;
          }
      };


      class dir : public saga::impl::impl_base
      {
        //////////////////////////////////////////////////////////////////////
        //
        // our dir adaptor base class
        //
        private:
          saga::util::shared_ptr <dir_instance_data>  idata_;

        public:
          dir (void);

          // as the constructor is also a cpi method, and we thus want to call the
          // cpi, we want to be finished with the actual object construction,
          // and the shared_ptr setup.  Thus impl construction is rendered as
          // a two-step process
          void_t constructor (std::string url);

          // instead of get_size calling each cpi individually, the call invocation
          // is passed off to the engine
          std::string get_url (void);

          saga::util::shared_ptr <saga::impl::filesystem::file> open (std::string url);

          // allow adaptor to obtain instance data (unlocked)
          saga::util::shared_ptr <dir_instance_data> get_instance_data (void)
          {
            return idata_;
          }

          void dump (std::string msg = "")
          {
            std::cout << "impl::dir (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
            idata_.dump ("    idata_      : ");
            idata_->dump();
          }
      }; // class dir

    } // namespace filesystem

  } // namespace impl


  namespace impl
  {
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
          std::cout << "functor (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
          std::cout << "    call        : " << saga::util::demangle (typeid (call_).name ()) << " : " << this->get_name () << std::endl;
          std::cout << "    IMPL        : " << saga::util::demangle (typeid (IMPL ).name ()) << std::endl;
          std::cout << "    CPI         : " << saga::util::demangle (typeid (CPI  ).name ()) << std::endl;
          std::cout << "    RET         : " << saga::util::demangle (typeid (RET  ).name ()) << std::endl;
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
          std::cout << "functor (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;
          std::cout << "    call        : " << saga::util::demangle (typeid (call_).name ()) << " : " << this->get_name () << std::endl;
          std::cout << "    ARG 1       : " << saga::util::demangle (typeid (ARG_1).name ()) << " : " << arg_1_      << std::endl;
          std::cout << "    IMPL        : " << saga::util::demangle (typeid (IMPL ).name ()) << std::endl;
          std::cout << "    CPI         : " << saga::util::demangle (typeid (CPI  ).name ()) << std::endl;
          std::cout << "    RET         : " << saga::util::demangle (typeid (RET  ).name ()) << std::endl;
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
    // or slightly more clever ;-)
    //
    class engine : public saga::util::shareable
    {
      private:
        std::vector <saga::util::shared_ptr <saga::impl::cpi_base> > cpis_;

      public:
        engine (void)
        {
          SAGA_UTIL_STACKTRACE ();
          std::cout << "engine: register all adaptors" << std::endl;

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
          std::vector <saga::util::shared_ptr <CPI> > cpis_ = get_cpis <CPI> ();

          // try one adaptor after the other, until one succeeds.
          for ( unsigned int i = 0; i < cpis_.size (); i++ )
          {
            try
            {
              saga::util::shared_ptr <func_base_t> base   = cc->get_func ();
              // saga::util::shared_ptr <func_cast_t> casted = base.get_shared_ptr <func_cast_t> ();

              base->call_cpi (cpis_[i], cc);
              cc->set_state (Done);
              std::cout << "adaptor " << i << " : succeeded for " << cc->get_func()->get_name () << std::endl;

              return;
            }
            catch ( const char * m )
            {
              // something went wrong...
              std::cout << "adaptor " << i << " : failed for " << cc->get_func()->get_name () << " : " << m << std::endl;
            }
            catch ( ... )
            {
              // something went wrong...
              std::cout << "adaptor " << i << " : failed for " << cc->get_func()->get_name () << " : ???" << std::endl;
            }
          }

          // no adaptor suceeded.  We don't have anything sensible to return, so
          // we flag the failure, and throw.  That is redundant, but hey...
          cc->set_state (Failed);

          std::cout << "all adaptors failed for " << cc->get_func()->get_name () << std::endl;
          throw "no adaptor suceeded";
        }

        void dump (std::string msg = "")
        {
          std::cout << "engine (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;

          for ( unsigned int i = 0; i < cpis_.size (); i++ )
          {
            cpis_[i].dump();
          }
        }
    };

  } // namespace impl




  //////////////////////////////////////////////////////////////////////
  //
  // two independent CPI implementations, aka adaptors.
  //
  namespace adaptor
  {
    namespace test // test adaptprs
    {
      class task_adaptor_0 : public saga::impl::task_cpi
      {
        private:
          typedef saga::impl::void_t             void_t;
          typedef saga::impl::task               api_t;
          typedef saga::impl::task_cpi           cpi_t;
          typedef saga::impl::task_instance_data idata_t;

          static void * threaded_cc (void * cc_sp)
          {
            std::cout << " @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;

            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <saga::impl::call_context> * cc_tmp
              = static_cast <saga::util::shared_ptr <saga::impl::call_context> *> (cc_sp);

            typedef saga::impl::task     api_t;
            typedef saga::impl::task_cpi cpi_t;
            typedef saga::impl::task_cpi ret_t;

            saga::util::shared_ptr <saga::impl::call_context> cc = cc_tmp->get_shared_ptr (); 

            std::cout << "thread created " << pthread_self () << std::endl;

            cc->dump ();

            // wait 'til task is getting run()
            while ( cc->get_task_state () == saga::impl::New )
            {
              ::sleep (1); // FIXME: nanosleep, configurable timeout
            }

            if ( cc->get_task_state () == saga::impl::Running )
            {
              std::cout << "thread starting " << pthread_self () << std::endl;

              saga::util::shared_ptr <saga::impl::impl_base> impl (cc->get_impl ()); 

              std::cout << " 1 xxxxxxxxxxxxxxx" << std::endl;

              impl->dump ("impl sp: ");

              std::cout << " 2 xxxxxxxxxxxxxxx" << std::endl;

              saga::util::shared_ptr <saga::impl::engine> engine = impl->get_engine ();

              engine.dump  ();
              engine->dump ();

              std::cout << " 3 xxxxxxxxxxxxxxx" << std::endl;

              engine->call <api_t, cpi_t> (cc); // this will set state

              std::cout << " 4 xxxxxxxxxxxxxxx" << std::endl;

              // the sync call is now Done, and its result is stored
            }

            std::cout << "thread done " << pthread_self () << std::endl;
          }


        public:
          task_adaptor_0 (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 

          ~task_adaptor_0 (void) 
          {
            SAGA_UTIL_STACKTRACE ();
          } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "task adaptor 0 : constructor ()" << std::endl;
            
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            std::cout << " ==idata cc =====================================================" << std::endl;
            idata->t_cc->dump ();
            std::cout << " ================================================================" << std::endl;

            // our new task exists, and idata->cc is the call it is operating
            // on.  That call's state depends on the runmode and the previous
            // state.
            //
            //   runmode   oldstate   newstate    notes
            //
            //   sync      new        done        need to run/wait
            //   sync      running    done        impossible
            //   sync      done       done
            //   sync      failed     failed
            //
            //   async     new        running     need to run
            //   async     running    running     
            //   async     done       done
            //   async     failed     failed
            //
            //   task      new        new
            //   task      running    running     
            //   task      done       done
            //   task      failed     failed
            //
            //   so, only 2 combinations require actions, and below we handle
            //   those

            // set call mode to Sync, because otherwise we will iterate
            // endlessly
            if ( idata->t_cc->get_mode  () == saga::impl::Sync &&
                 idata->t_cc->get_task_state () == saga::impl::New  )
            {
              std::cout << " == sync task =====================================================" << std::endl;
              impl->get_engine ()->call <api_t, cpi_t> (idata->t_cc); // this will set state
            }
            else if ( idata->t_cc->get_mode  () == saga::impl::Async &&
                      idata->t_cc->get_task_state () == saga::impl::New   )
            {
              std::cout << " == async task =====================================================" << std::endl;
              pthread_t      thread;
              pthread_attr_t att;

              std::cout << " task adaptor 0 : create new thread " << std::endl;

              saga::util::shared_ptr <saga::impl::call_context> * tmp = new saga::util::shared_ptr <saga::impl::call_context> (idata->t_cc);

              int err = pthread_create (&thread, NULL,
                                        saga::adaptor::test::task_adaptor_0::threaded_cc, 
                                        (void*)&(idata->t_cc));

              // int err = pthread_create (&thread, NULL,
              //                           saga::adaptor::test::task_adaptor_0::threaded_cc, 
              //                           (void*)tmp);

              if ( 0 != err )
              {
                std::cout << " @@@ could not create thread: " << ::strerror (err) <<
                  std::endl;
                throw "oops";
              }

              idata->t_cc->set_task_state (saga::impl::Running);
            }

            cc->set_task_state (saga::impl::Done);

            return;
          } 

          void get_state (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();

            typedef saga::impl::call_state res_t;

            saga::util::shared_ptr <saga::impl::task> impl (cc->get_impl ()); 

            std::cout << "task adaptor 0 : get_state ()" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            cc->set_result <res_t> (idata->t_cc->get_task_state ());
            cc->set_task_state (saga::impl::Done);

            return;
          }

          void get_result (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();

            typedef saga::util::shared_ptr <saga::impl::result_t> res_t;

            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "task adaptor 0 : get_result ()" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            std::cout << "=-----------------------------------------------" << std::endl;
            idata->t_cc->dump ();
            std::cout << "=-----------------------------------------------" << std::endl;

            cc->set_result <res_t> (idata->t_cc->get_result ());
            cc->set_task_state (saga::impl::Done);

            return;
          }
      };


      class file_adaptor_0 : public saga::impl::filesystem::file_cpi
      {
        private:
          typedef saga::impl::void_t                         void_t;
          typedef saga::impl::filesystem::file               api_t;
          typedef saga::impl::filesystem::file_cpi           cpi_t;
          typedef saga::impl::filesystem::file_instance_data idata_t;

        public:
          file_adaptor_0    (void) { } 
          ~file_adaptor_0   (void) { } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc, 
                              std::string                           url) 
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "file adaptor 0 : constructor (" << url << ")" << std::endl;
            
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            cc->set_task_state (saga::impl::Done);

            return;
          } 

          void get_size (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            cc->set_task_state (saga::impl::Failed);

            std::cout << "file adaptor 0 : get_size" << std::endl;
            throw "oops";
          }

          void get_size (saga::util::shared_ptr <saga::impl::call_context> cc, 
                         saga::impl::call_mode                             m)
          { 
            SAGA_UTIL_STACKTRACE ();

            cc->set_task_state (saga::impl::Failed);

            std::cout << "file adaptor 0 : get_size <async>" << std::endl;
            throw "oops";

          } 

          void copy (saga::util::shared_ptr <saga::impl::call_context> cc,
                     std::string                                       tgt)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            cc->set_task_state (saga::impl::Failed);

            std::cout << "file adaptor 0 : copy " << tgt << std::endl;
            throw "oops";
            return;
          }
      };


      class file_adaptor_1 : public saga::impl::filesystem::file_cpi
      {
        private:
          typedef saga::impl::void_t                         void_t;
          typedef saga::impl::filesystem::file               api_t;
          typedef saga::impl::filesystem::file_cpi           cpi_t;
          typedef saga::impl::filesystem::file_instance_data idata_t;

        public:
          file_adaptor_1    (void) { } 
          ~file_adaptor_1   (void) { } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                            std::string                                       url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "file adaptor 1 : constructor (" << url << ")" << std::endl;
            
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            cc->set_task_state (saga::impl::Done);

            return;
          } 

          void get_size (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "file adaptor 1 : get_size ()" << std::endl;
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            struct stat buf;
            (void) stat (idata->url.c_str (), &buf);

            cc->set_result <int> (buf.st_size);
            cc->set_task_state (saga::impl::Done);
          }

          void get_size (saga::util::shared_ptr <saga::impl::call_context> cc, 
                         saga::impl::call_mode                             m)
          { 
            SAGA_UTIL_STACKTRACE ();
            std::cout << " ===== file adaptor 1 : get_size <> ()" << std::endl;

            cc->set_mode (m);

            cc->dump ();

            if ( m == saga::impl::Sync )
            {
              std::cout << " ===== file adaptor 1 : get_size <Sync> ()" << std::endl;

              // call the normal sync call, 
              // this is setting result and state
              get_size (cc); 
              cc->set_task_state (saga::impl::Done);

              saga::util::shared_ptr <saga::impl::task> ret (new saga::impl::task (cc));

              cc->set_result <saga::util::shared_ptr <saga::impl::task> > (ret);

              std::cout << " ===== get_size <Sync> () done ===== " << std::endl;
              return;
            }
            else if ( m == saga::impl::Async ||
                      m == saga::impl::Task  )
            {
              std::cout << " ===== file adaptor 1 : get_size <Async> ()" << std::endl;

              // async version: create a task straight away, and let the task 
              // adaptor deal with the async invocation of the sync call
              
              saga::util::shared_ptr <saga::impl::task> ret (new saga::impl::task (cc));

              cc->set_task_state (saga::impl::Done);

              cc->set_result <saga::util::shared_ptr <saga::impl::task> > (ret);

              std::cout << " ===== get_size <Async> () done ===== " << std::endl;

              return;
            }

            throw "incorrect call mode!? - that should never happen!";
          } 

          void copy (saga::util::shared_ptr <saga::impl::call_context> cc,
                     std::string                                       tgt)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "file adaptor 1 : copy " << tgt << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            int res = ::system ((std::string ("cp ") + idata->url + " " + tgt).c_str ());

            if ( res != 0 )
            {
              cc->set_task_state (saga::impl::Failed);
              // cc->set_error ("system command error"); // TODO
            }
            else
            {
              cc->set_task_state (saga::impl::Done);
            }

            return;
          }
      };


      class dir_adaptor_0 : public saga::impl::filesystem::dir_cpi
      {
        private:
          typedef saga::impl::void_t                        void_t;
          typedef saga::impl::filesystem::dir               api_t;
          typedef saga::impl::filesystem::dir_cpi           cpi_t;
          typedef saga::impl::filesystem::dir_instance_data idata_t;

        public:
          dir_adaptor_0    (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 
          
          ~dir_adaptor_0   (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                            std::string                                       url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "dir adaptor 0 : constructor (" << url << ")" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            cc->set_task_state (saga::impl::Done);

            return;
          } 

          void get_url (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

            std::cout << "dir adaptor 0 : get_url" << std::endl;
            throw "oops";
          }
      };

      class dir_adaptor_1 : public saga::impl::filesystem::dir_cpi
      {
        private:
          typedef saga::impl::void_t                        void_t;
          typedef saga::impl::filesystem::dir               api_t;
          typedef saga::impl::filesystem::dir_cpi           cpi_t;
          typedef saga::impl::filesystem::dir_instance_data idata_t;

        public:
          dir_adaptor_1    (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 

          ~dir_adaptor_1   (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                            std::string                                       url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "dir adaptor 1 : constructor (" << url << ")" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            cc->set_task_state (saga::impl::Done);

            return;
          } 

          void get_url (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            std::cout << "dir adaptor 1 : get_url: " << idata->url << std::endl;

            cc->set_result <std::string> (idata->url);
            cc->set_task_state (saga::impl::Done);

            return;
          }


          void open (saga::util::shared_ptr <saga::impl::call_context> cc, 
                     std::string                                       url)
          {
            SAGA_UTIL_STACKTRACE ();

            typedef saga::util::shared_ptr <saga::impl::filesystem::file> res_t;

            saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            std::string new_url = idata->url + "/" + url;

            std::cout << "dir adaptor 1 : open: " << new_url << std::endl;

            // FIXME: error checks
            res_t ret (new saga::impl::filesystem::file);

            // FIXME: error checks
            // FIXME: should constructor be called by the impl::dir::open, or
            // here in the adaptor?  In the impl it can go to any adaptor, from
            // here as well...
            ret->constructor (new_url);

            cc->set_result <res_t> (ret);
            cc->set_task_state (saga::impl::Done);

            return;
          }
      };

    } // namespace test

  } // namespace adaptor


  //////////////////////////////////////////////////////////////////////
  //
  // API classes
  //
  class task
  {
    private:
      saga::util::shared_ptr <saga::impl::task> impl_;

    public:
      task (void)
        : impl_ (new saga::impl::task)
      {
        SAGA_UTIL_STACKTRACE ();
        (void) impl_->constructor ();
      }

      task (saga::util::shared_ptr <saga::impl::task> impl)
        : impl_ (impl)
      {
        SAGA_UTIL_STACKTRACE ();
        (void) impl_->constructor ();
      }

      saga::impl::call_state get_state (void)
      {
        SAGA_UTIL_STACKTRACE ();
        return impl_->get_state ();
      }

      template <typename T>
      T get_result (void)
      {
        SAGA_UTIL_STACKTRACE ();
        saga::util::shared_ptr <saga::impl::result_t> result = impl_->get_result (); 
        saga::util::shared_ptr <saga::impl::result_t_detail_ <T> > casted = result.get_shared_ptr <saga::impl::result_t_detail_ <T> > ();
        // FIXME: error check
        std::cout << " ##############################################################" << std::endl;
        std::cout << " ------------------------------------ " << saga::util::demangle (typeid (T).name ()) << std::endl;
        impl_.dump ();
        std::cout << " ------------------------------------ " << std::endl;
        impl_->dump ();
        std::cout << " ##############################################################" << std::endl;

        return casted->get ();
      }
  };


  namespace filesystem
  {
    class file
    {
      private:
        saga::util::shared_ptr <saga::impl::filesystem::file> impl_;

      public:
        file (std::string url)
          : impl_ (new saga::impl::filesystem::file)
        {
          SAGA_UTIL_STACKTRACE ();
          (void) impl_->constructor (url);
        }

        file (saga::util::shared_ptr <saga::impl::filesystem::file> impl)
          : impl_ (impl)
        {
          SAGA_UTIL_STACKTRACE ();
          // FIXME: where to get the URL/idata?  Is constructor here really useful,
          // as impl already exists?  Is impl always initialized?
          // In general, need to clear up who is filling idata, when the object
          // is adaptor initialized / bound (void) 
          //
          // impl_->constructor (url);
        }

        int get_size (void)
        {
          SAGA_UTIL_STACKTRACE ();
          return impl_->get_size ();
        }

        template <enum saga::impl::call_mode M>
        saga::task get_size (void)
        {
          SAGA_UTIL_STACKTRACE ();
          return saga::task (impl_->get_size (M));
        }

        void copy (std::string tgt)
        {
          SAGA_UTIL_STACKTRACE ();
          (void) impl_->copy (tgt);
        }
    };

    class dir
    {
      private:
        saga::util::shared_ptr <saga::impl::filesystem::dir> impl_;

      public:
        dir (std::string url)
          : impl_ (new saga::impl::filesystem::dir)
        {
          SAGA_UTIL_STACKTRACE ();
          (void) impl_->constructor (url);
        }

        std::string get_url (void)
        {
          SAGA_UTIL_STACKTRACE ();
          return impl_->get_url ();
        }

        saga::filesystem::file open (std::string url)
        {
          SAGA_UTIL_STACKTRACE ();
          return saga::filesystem::file (impl_->open (url));
        }
    };

  } // namespace filesystem

} // namespace saga

#endif //  SAGA_CPI_4_HPP

