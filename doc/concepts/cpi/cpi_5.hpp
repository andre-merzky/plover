
#ifndef SAGA_CPI_4_HPP
#define SAGA_CPI_4_HPP

#include <map>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include <saga/util/mutex.hpp>
#include <saga/util/scoped_lock.hpp>
#include <saga/util/shareable.hpp>
#include <saga/util/enums.hpp>

#define STR(x) #x

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
    };

    //////////////////////////////////////////////////////////////////
    //
    // the result type thingies allow to keep get_result template free on impl
    // level and below - the template free base class is used wherever possible, 
    // and the types result_t_detail_ is only aquired as needed.
    //
    class result_t 
    {
      public:
        virtual ~result_t (void) {}
    };

    template <typename T>
    class result_t_detail_ : public result_t
    {
      public:
        T t;
    };


    enum cpi_mode {
      Any        = 0, 
      Bounde     = 1, 
      Simple     = 2, 
      Collective = 3, 
      Filter     = 4,
      All        = 5
    };

    REGISTER_ENUM (cpi_mode, Any       , 0);
    REGISTER_ENUM (cpi_mode, Bounde    , 1);
    REGISTER_ENUM (cpi_mode, Simple    , 2);
    REGISTER_ENUM (cpi_mode, Collective, 3);
    REGISTER_ENUM (cpi_mode, Filter    , 4);
    REGISTER_ENUM (cpi_mode, All       , 5);


    enum call_mode  // FIXME: eventually becomes saga::task::mode
    { 
      Sync  = 0,
      Async = 1,
      Task  = 2
    };

    // REGISTER_ENUM (call_mode, Sync , 0);
    // REGISTER_ENUM (call_mode, Async, 1);
    // REGISTER_ENUM (call_mode, Task , 2);


    enum call_state { // FIXME: eventually becomes saga::task::state
      Unknown    = 0,
      New        = 1,
      Running    = 2,
      Done       = 3,
      Failed     = 4,
      Canceled   = 5
    };

    REGISTER_ENUM (call_state, Unknown , 0);
    REGISTER_ENUM (call_state, New     , 1);
    REGISTER_ENUM (call_state, Running , 2);
    REGISTER_ENUM (call_state, Done    , 3);
    REGISTER_ENUM (call_state, Failed  , 4);
    REGISTER_ENUM (call_state, Canceled, 5);

    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all functors
    //
    class functor_base : public saga::util::shareable
    {
      public: 
        functor_base (void) 
        {
          // std::cout << "functor_base ctor " << this << std::endl;
        }

        virtual ~functor_base (void) 
        {
          // std::cout << "functor_base dtor " << this << std::endl;
        }
    };

    template <typename IMPL, typename CPI, typename RET>
    class functor : public functor_base
    {
      private:
        typedef functor <IMPL, CPI, RET> func_t;

        RET result_; // return value

      public:
        // virtual ~functor (void) = 0;
        virtual RET call_cpi (saga::util::shared_ptr <CPI>    cpi, 
                              saga::util::shared_ptr <saga::impl::call_context> cc) = 0;
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
    //  saga::exception                       exception_;       // exception stack collected from adaptors_used_/failed
    //  saga::util::timestamp                 created_;         // created time stamp
    //  saga::util::timestamp                 start_;           // start time stamp
    //  saga::util::timestamp                 stop_;            // stop time stamp
    //  saga::util::timestamp                 duration_;        // time needed for completion
        std::string                           func_name_;       // name of method which created functor
        std::string                           func_args_;       // for logging, args given to functor
    //  std::vector <saga::util::log::entry>  log_;             // some audit log
        saga::impl::result_t                  result_;          // container for task result type and value

        saga::util::shared_ptr <functor_base> func_;            // the functor to call

      public:
        call_context (saga::util::shared_ptr <functor_base> func, 
                      saga::util::shared_ptr <shareable>    impl)
          : func_  (func)
          , impl_  (impl)
          , state_ (New)
        {
        }

        saga::util::shared_ptr <functor_base> get_func (void)           { return func_;     } 
        saga::util::shared_ptr <shareable>    get_impl (void)           { return impl_;     } 

        void                   set_state     (saga::impl::call_state s) { state_ = s;       }
        saga::impl::call_state get_state     (void                    ) { return state_;    }

        void                   set_func_name (std::string fn)           {func_name_ = fn;   }
        std::string            get_func_name (void)                     {return func_name_; }

        void                   set_result    (saga::impl::result_t r)   { result_ = r;      }
        saga::impl::result_t   get_result    (void)                     { return result_;   }
    };


    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all CPIs
    //
    class cpi_base : public saga::util::shareable
    {
      private:
          std::map <std::string, void*> adata_;

      public:
        virtual ~cpi_base (void) {}
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

        virtual void_t constructor (saga::util::shared_ptr <call_context> cc)
        { 
          throw "constructor: NotImplemented"; 
        } 

        virtual saga::impl::call_state get_state (saga::util::shared_ptr <call_context> cc)
        {
          throw "get_state : NotImplemented"; 
        }

        // FIXME: cont here (any needed)
        virtual saga::impl::result_t get_result (saga::util::shared_ptr <call_context> cc)
        { 
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
        task_instance_data (void)
        {
          std::cout << "task idata ctor" << std::endl;
        }

        ~task_instance_data (void)
        {
          std::cout << "task idata dtor" << std::endl;
        }

        saga::util::shared_ptr <saga::impl::call_context> cc;
    };


    class task : public saga::util::shareable
    {
      private:
        saga::util::shared_ptr <saga::impl::engine> engine_;
        saga::util::shared_ptr <task_instance_data> idata_;

      public:
        task (void);
        task (saga::util::shared_ptr <saga::impl::call_context> cc);

        void_t                 constructor (void);
        saga::impl::call_state get_state   (void);
        result_t               get_result  (void);

        // allow adaptor to obtain instance data (unlocked)
        saga::util::shared_ptr <task_instance_data> get_instance_data (void)
        {
          return idata_;
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
          virtual ~file_cpi (void) { };

          virtual void_t constructor (saga::util::shared_ptr <call_context> cc, 
                                      std::string                           url) 
          { 
            throw "constructor: NotImplemented"; 
          } 

          virtual int get_size (saga::util::shared_ptr <call_context> cc)
          { 
            throw "get_size : NotImplemented"; 
          } 

          //  we have a second get_size method for the async versions.  The
          //  call_mode parameter is somewhat redundant, as the cm is also
          //  stored in the cc, but it allows to use overloading in the adaptor
          //  for the various sync/async calls.  The adaptor needs to switch
          //  over the enum to see what async flavor is wanted / needed, 
          //  but the returned task's state can easily be adjusted by the
          //  calling functor or by the engine.
          virtual saga::util::shared_ptr <saga::impl::task> get_size (saga::util::shared_ptr <call_context> cc, 
                                                                      saga::impl::call_mode                 m)
          { 
            throw "get_size <...> : NotImplemented"; 
          } 

          virtual void_t copy (saga::util::shared_ptr <call_context> cc, 
                               std::string                           tgt)
          {
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
      };


      // saga impl class
      class file : public saga::util::shareable
      {
        private:
          saga::util::shared_ptr <saga::impl::engine> engine_;
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
      }; // class file

      ///////////////////////////////////////////////////////////////////////////////////////////

      class dir_cpi : public saga::impl::cpi_base
      {
        public:
          virtual ~dir_cpi (void) { };

          virtual void_t constructor (saga::util::shared_ptr <call_context> cc, 
                                      std::string                           url) 
          { 
            throw "constructor: NotImplemented"; 
          } 

          virtual std::string get_url (saga::util::shared_ptr <call_context> cc)
          { 
            throw "get_url : NotImplemented"; 
          } 

          virtual saga::util::shared_ptr <saga::impl::filesystem::file> open (saga::util::shared_ptr <call_context> cc, 
                                                                              std::string                           url)
          {
            throw "open : NotImplemented"; 
          }
      };

      // saga api class
      class dir_instance_data : public saga::util::shareable
      {
        private:
          saga::util::mutex mtx_;
          // saga::session s_;

        public:
          std::string url;
          size_t      pos;
          bool        valid;
      };


      class dir : public saga::util::shareable
      {
        // saga::impl::dir_instance_data manages the state information for one
        // specific saga::dir instance.  Note that this state is shared by all
        // dir adaptors: they can obtained a scoped-locked copy of it via
        // get_instance_data()
        public:

          //////////////////////////////////////////////////////////////////////
          //
          // our dir adaptor base class
          //
        private:
          saga::util::shared_ptr <saga::impl::engine> engine_;
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

      }; // class dir

    } // namespace filesystem

  } // namespace impl


  //////////////////////////////////////////////////////////////////////
  //
  // two independent CPI implementations, aka adaptors.
  //
  namespace adaptor
  {
    namespace test // test adaptprs
    {
      class file_adaptor_0 : public saga::impl::filesystem::file_cpi
      {
        private:
          typedef saga::impl::void_t                         void_t;
          typedef saga::impl::filesystem::file               api_t;
          typedef saga::impl::filesystem::file_cpi           cpi_t;
          typedef saga::impl::filesystem::file_instance_data idata_t;

        public:
          file_adaptor_0    (void) { std::cout << "file adaptor 0 : ctor" << std::endl; } 
          ~file_adaptor_0   (void) { std::cout << "file adaptor 0 : dtor" << std::endl; } 

          void_t constructor (saga::util::shared_ptr <saga::impl::call_context> cc, 
                              std::string                           url) 
          {
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "file adaptor 0 : constructor (" << url << ")" << std::endl;
            
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            return void_t ();
          } 

          int get_size (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "file adaptor 0 : get_size" << std::endl;
            throw "oops";
          }

          saga::util::shared_ptr <saga::impl::task> get_size (saga::util::shared_ptr <saga::impl::call_context> cc, 
                                                              saga::impl::call_mode                             m)
          { 
            std::cout << "file adaptor 0 : get_size <async>" << std::endl;
            throw "oops";
          } 

          void_t copy (saga::util::shared_ptr <saga::impl::call_context> cc,
                       std::string                                       tgt)
          {
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "file adaptor 0 : copy " << tgt << std::endl;
            throw "oops";
            return void_t ();
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
          file_adaptor_1    (void) { std::cout << "file adaptor 1 : ctor" << std::endl; } 
          ~file_adaptor_1   (void) { std::cout << "file adaptor 1 : dtor" << std::endl; } 

          void_t constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                              std::string                                       url) 
          { 
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "file adaptor 1 : constructor (" << url << ")" << std::endl;
            
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            return void_t ();
          } 

          int get_size (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "file adaptor 1 : get_size" << std::endl;
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            struct stat buf;
            (void) stat (idata->url.c_str (), &buf);

            return buf.st_size;
          }

          saga::util::shared_ptr <saga::impl::task> get_size (saga::util::shared_ptr <saga::impl::call_context> cc, 
                                                              saga::impl::call_mode                             m)
          { 
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "file adaptor 1 : get_size <async>" << std::endl;
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            std::cout << " ===== creating new impl task ===== " << std::endl;
            saga::util::shared_ptr <saga::impl::task> ret (new saga::impl::task (cc));

            return ret;
          } 

          void_t copy (saga::util::shared_ptr <saga::impl::call_context> cc,
                       std::string                                       tgt)
          {
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "file adaptor 1 : copy " << tgt << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            int res = ::system ((std::string ("cp ") + idata->url + " " + tgt).c_str ());

            if ( res != 0 )
            {
              throw "system command error";
            }

            return void_t ();
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
          dir_adaptor_0    (void) { std::cout << "dir adaptor 0 : ctor" << std::endl; } 
          ~dir_adaptor_0   (void) { std::cout << "dir adaptor 0 : dtor" << std::endl; } 

          void_t constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                              std::string                                       url) 
          { 
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "dir adaptor 0 : constructor (" << url << ")" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            return void_t ();
          } 

          std::string get_url (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
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
          dir_adaptor_1    (void) { std::cout << "dir adaptor 1 : ctor" << std::endl; } 
          ~dir_adaptor_1   (void) { std::cout << "dir adaptor 1 : dtor" << std::endl; } 

          void_t constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                              std::string                                       url) 
          { 
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "dir adaptor 1 : constructor (" << url << ")" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            return void_t ();
          } 

          std::string get_url (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            std::cout << "dir adaptor 1 : get_url: " << idata->url << std::endl;

            return idata->url;
          }


          saga::util::shared_ptr <saga::impl::filesystem::file> open (saga::util::shared_ptr <saga::impl::call_context> cc, 
                                                                      std::string                                       url)
          {
            saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            std::string new_url = idata->url + "/" + url;

            std::cout << "dir adaptor 1 : open: " << new_url << std::endl;

            // FIXME: error checks
            saga::util::shared_ptr <saga::impl::filesystem::file> ret (new saga::impl::filesystem::file);

            // FIXME: error checks
            // FIXME: should constructor be called by the impl::dir::open, or
            // here in the adaptor?  In the impl it can go to any adaptor, from
            // here as well...
            ret->constructor (new_url);

            return ret;
          }
      };

      class task_adaptor_0 : public saga::impl::task_cpi
      {
        private:
          typedef saga::impl::void_t             void_t;
          typedef saga::impl::task               api_t;
          typedef saga::impl::task_cpi           cpi_t;
          typedef saga::impl::task_instance_data idata_t;

        public:
          task_adaptor_0    (void) { 
            std::cout << " ===== creating new task instance ===== " << std::endl;
            std::cout << "task adaptor 0 : ctor" << std::endl; 
          } 
          ~task_adaptor_0   (void) { std::cout << "task adaptor 0 : dtor" << std::endl; } 

          void_t constructor (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "task adaptor 0 : constructor ()" << std::endl;
            
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            saga::util::shared_ptr <saga::impl::call_context> t_cc = idata->cc;

            std::cout << " ===== initializing new task instance ===== " << idata->cc->get_func_name () << std::endl;

            t_cc->set_state (saga::impl::Running);

            return void_t ();
          } 

          saga::impl::call_state get_state (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            saga::util::shared_ptr <saga::impl::task> impl (cc->get_impl ()); 

            std::cout << "task adaptor 0 : get_state ()" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            saga::util::shared_ptr <saga::impl::call_context> t_cc = idata->cc;

            saga::impl::call_state ret = t_cc->get_state ();

            return ret;
          }

          saga::impl::result_t get_result (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            std::cout << "task adaptor 0 : get_result ()" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            saga::util::shared_ptr <saga::impl::call_context> t_cc = idata->cc;

            saga::impl::result_t ret = t_cc->get_result ();

            return ret;
          }
      };



    } // namespace test

  } // namespace adaptor


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
        typedef functor <IMPL, CPI, RET> func_t;

        RET (CPI::*call_)(saga::util::shared_ptr <call_context>);


      public: 
        functor_0 (RET (CPI::*call )(saga::util::shared_ptr <call_context>))
        { 
          call_  = call; 
        }

        RET call_cpi (saga::util::shared_ptr <CPI>                      cpi, 
                      saga::util::shared_ptr <saga::impl::call_context> cc)
        { 
          return ((*cpi).*(call_)) (cc); 
        }

        void dump (std::string msg = "")
        {
          std::cout << " === dump functor : call   : " << saga::util::demangle (typeid (call_).name ()) << std::endl;
          std::cout << "                    IMPL   : " << saga::util::demangle (typeid (IMPL ).name ()) << std::endl;
          std::cout << "                    CPI    : " << saga::util::demangle (typeid (CPI  ).name ()) << std::endl;
          std::cout << "                    RET    : " << saga::util::demangle (typeid (RET  ).name ()) << std::endl;
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
        typedef functor <IMPL, CPI, RET> func_t;

        RET (CPI::*call_)(saga::util::shared_ptr <call_context>, ARG_1); 

        ARG_1 arg_1_;

      public: 
        functor_1 (RET (CPI::*call )(saga::util::shared_ptr <call_context>, ARG_1), ARG_1 arg_1) 
        { 
          call_  = call; 
          arg_1_ = arg_1; 
        }

        RET call_cpi (saga::util::shared_ptr <CPI>    cpi, 
                      saga::util::shared_ptr <saga::impl::call_context> cc)
        { 
          return ((*cpi).*(call_)) (cc, arg_1_); 
        }

        void dump (std::string msg = "")
        {
          std::cout << " === dump functor : call   : " << saga::util::demangle (typeid (call_).name ()) << std::endl;
          std::cout << "                    IMPL   : " << saga::util::demangle (typeid (IMPL ).name ()) << std::endl;
          std::cout << "                    CPI    : " << saga::util::demangle (typeid (CPI  ).name ()) << std::endl;
          std::cout << "                    RET    : " << saga::util::demangle (typeid (RET  ).name ()) << std::endl;
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
        template <typename IMPL, typename CPI, typename RET>
        RET call (saga::util::shared_ptr <saga::impl::call_context> cc)
        {
          typedef saga::impl::functor_base             func_base_t;
          typedef saga::impl::functor <IMPL, CPI, RET> func_cast_t;

          // get the matching list of CPIs
          std::vector <saga::util::shared_ptr <CPI> > cpis_ = get_cpis <CPI> ();

          cc->set_state (Running);

          // try one adaptor after the other, until one succeeds.
          for ( unsigned int i = 0; i < cpis_.size (); i++ )
          {
            try
            {
              saga::util::shared_ptr <func_base_t> base   = cc->get_func ();
              saga::util::shared_ptr <func_cast_t> casted = base.get_shared_ptr <func_cast_t> ();

              RET ret = casted->call_cpi (cpis_[i], cc);

              cc->set_state (Done);
              std::cout << "adaptor " << i << " : succeeded for " << cc->get_func_name () << std::endl;

              return ret;
            }
            catch ( const char * m )
            {
              // something went wrong...
              std::cout << "adaptor " << i << " : failed for " << cc->get_func_name () << " : " << m << std::endl;
            }
            catch ( ... )
            {
              // something went wrong...
              std::cout << "adaptor " << i << " : failed for " << cc->get_func_name () << " : ???" << std::endl;
            }
          }

          // no adaptor suceeded.  We don't have anything sensible to return, so
          // we flag the failure, and throw.  That is redundant, but hey...
          cc->set_state (Failed);

          std::cout << "all adaptors failed for " << cc->get_func_name () << std::endl;
          throw "no adaptor suceeded";
        }
    };

  } // namespace impl


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
        (void) impl_->constructor ();
      }

      task (saga::util::shared_ptr <saga::impl::task> impl)
        : impl_ (impl)
      {
        (void) impl_->constructor ();
      }

      saga::impl::call_state get_state (void)
      {
        return impl_->get_state ();
      }

      template <typename T>
      T get_result (void)
      {
        saga::impl::result_t_detail_ <T> res = static_cast <T> (impl_->get_result ()); 

        return res.t;
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
          (void) impl_->constructor (url);
        }

        file (saga::util::shared_ptr <saga::impl::filesystem::file> impl)
          : impl_ (impl)
        {
          // FIXME: where to get the URL/idata?  Is coinstructor here really useful,
          // as impl already exists?  Is it initialized?
          // In general, need to clear up who is filling idata, when is adaptor
          // initialized / bound
          // (void) impl_->constructor (url);
        }

        int get_size (void)
        {
          return impl_->get_size ();
        }

        template <enum saga::impl::call_mode M>
        saga::task get_size (void)
        {
          return saga::task (impl_->get_size (M));
        }

        void copy (std::string tgt)
        {
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
          (void) impl_->constructor (url);
        }

        std::string get_url (void)
        {
          return impl_->get_url ();
        }

        saga::filesystem::file open (std::string url)
        {
          return saga::filesystem::file (impl_->open (url));
        }
    };

  } // namespace filesystem

} // namespace saga

#endif //  SAGA_CPI_4_HPP

