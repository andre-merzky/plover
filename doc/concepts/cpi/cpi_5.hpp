
#ifndef SAGA_CPI_4_HPP
#define SAGA_CPI_4_HPP

#include <map>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include <saga/util/mutex.hpp>
#include <saga/util/shared_ptr.hpp>

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
//   filter-adaptors
//     adaptors may do something to the call's state and instance
//     data, and then call into the engine again.  An example might
//     be task_container, or basic parameter and state checks
//
//   asynchronous operations
//     make api function calls stateful things (aka tasks)
//
//   cpi-call modes
//     some calls may require to collect results over multiple
//     adaptors (js.list_jobs()), others may use adaptors for 
//     just filtering, calling one adaptor after the other 
//     (task_container).  
//
//  For those features, it seems prodent to extent the concept of Functors, and
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
//  In cpi_4, the interactions between functor and adaptor are coordinated
//  byt the engine (engine selects adaptor to use by functor).  That
//  functionality moves into the functor itself, making the engine even 
//  simpler: the engine 
//
//    - loads adaptors
//    - passes list of suitable adaptors to functor
//    - passes itself to functor, so that functor can do call scheduling
//      on its content, according to call mode, and 
//
//  To simplify and unify functors and cpi function pointers, we use bool 
//  functions everywhere below the top level API.  The return value is stored 
//  as reference in the functor itself (think task.get_result ()).
//
//
//
namespace saga
{
  //////////////////////////////////////////////////////////////////
  //
  // prototypes and declarations
  //
  // note that the structure still differs from the actual code structure, in
  // order to keep this example within a single source file.
  //

  namespace impl
  {
    namespace filesystem
    {
      class file;
      class file_cpi;
      class file_instance_data;
    }
  }
  namespace adaptor
  {
    namespace test
    {
      class adaptor_0;
      class adaptor_1;
    }
  }
  namespace impl
  {
    template <typename IMPL, typename CPI, typename RET>                 class functor;
    template <typename IMPL, typename CPI, typename RET>                 class functor_0;
    template <typename IMPL, typename CPI, typename RET, typename ARG_1> class functor_1;
    class engine;
  }
  namespace filesystem
  {
    class file;
  }


  namespace impl
  {
    class void_t { };
    template <typename IMPL, typename CPI, typename RET>                 class functor_0;
    template <typename IMPL, typename CPI, typename RET, typename ARG_1> class functor_1;


    enum cpi_call_mode {
      Any        = 0, 
      Bounde     = 1, 
      Simple     = 2, 
      Collective = 3, 
      Filter     = 4,
      All        = 5
    };

    enum async_call_mode {
      Sync       = 0,
      ASync      = 1,
      Task       = 2
    };

    //////////////////////////////////////////////////////////////////
    //
    // father of all functors
    //
    template <typename IMPL, typename CPI, typename RET>
    class functor : public saga::util::shareable
    {
      private:
        typedef functor <IMPL, CPI, RET> func_t;

        std::vector <std::string>             adaptors_used_;   // adaptors which have been used (audit trail)
        std::vector <std::string>             adaptors_;        // adaptors to use
        std::vector <std::string>             adaptors_skip_;   // adaptors not to use
        saga::util::shared_ptr <IMPL>         impl_;            // calling object (has session)
        saga::impl::cpi_call_mode             cpi_call_mode_;   // also as class templates?
        saga::impl::async_call_mode           async_call_mode_; // also as class templates?
    //  saga::exception                       exception_;       // exception stack collected from adaptors_used_/failed
    //  saga::task::state                     state_;           // current state
    //  saga::util::timestamp                 start_;           // start time stamp
    //  saga::util::timestamp                 stop_;            // stop time stamp
    //  saga::util::timestamp                 duration_;        // time needed for completion
        std::string                           method_name_;     // name of method which created functor
        std::string                           method_args_;     // for logging, args given to functor
    //  std::vector <saga::util::log::entry>  log_;             // some audit log
        RET                                   result_;          // return value
        saga::util::mutex                     mtx_;             // mult.  adaptors etc run concurrently

      public:
        // virtual ~functor (void) = 0;
        virtual RET call_cpi (saga::util::shared_ptr <func_t> func, 
                              saga::util::shared_ptr <CPI>    cpi, 
                              saga::util::shared_ptr <IMPL>   impl) = 0;
    };


    //////////////////////////////////////////////////////////////////
    //
    // the filesystem implementation
    //
    namespace filesystem
    {
      class file_cpi : public saga::util::shareable
      {
        public:
          virtual ~file_cpi (void) { };

          virtual void_t constructor (
              saga::util::shared_ptr <functor <saga::impl::filesystem::file, 
                                               saga::impl::filesystem::file_cpi, void_t> >   func, 
              saga::util::shared_ptr <saga::impl::filesystem::file>    impl, 
              std::string                                              url) 
          { 
            throw "constructor: NotImplemented"; 
          } 

          virtual int get_size (
              saga::util::shared_ptr <functor <saga::impl::filesystem::file, 
                                               saga::impl::filesystem::file_cpi, 
                                               int> >                  func,
              saga::util::shared_ptr <saga::impl::filesystem::file>    impl)
          { 
            throw "get_size : NotImplemented"; 
          } 

          virtual void_t copy (
              saga::util::shared_ptr <functor <saga::impl::filesystem::file, 
                                               saga::impl::filesystem::file_cpi, void_t> >   func, 
              saga::util::shared_ptr <saga::impl::filesystem::file>    impl, 
              std::string                                              tgt)
          {
            throw "copy : NotImplemented"; 
          }
      };

      // saga api class
      class file_instance_data : public saga::util::shareable
      {
        private:
          saga::util::mutex mtx_;
          std::map <std::string, void*> adata_;
          // saga::session s_;

        public:
          std::string url;
          size_t      pos;
          bool        valid;

          saga::util::mutex & get_mutex (void)
          {
            return mtx_;
          }
      };


      class file : public saga::util::shareable
      {
        // saga::impl::file_instance_data manages the state information for one
        // specific saga::file instance.  Note that this state is shared by all
        // file adaptors: they can obtained a scoped-locked copy of it via
        // get_instance_data()
        public:

          //////////////////////////////////////////////////////////////////////
          //
          // our file adaptor base class
          //
        private:
          saga::util::shared_ptr <saga::impl::engine> engine_;
          saga::util::shared_ptr <file_instance_data> idata_;

        public:
          file (void);

          // as the constructor is also a cpi method, and we thus want to call the
          // cpi, we want to be finished with the actual object construction,
          // and the shared_ptr setup.  Thus impl construction is rendered as
          // a two-step process
          void_t constructor (std::string url);

          // instead of get_size calling each cpi individually, the call invocation
          // is passed off to the engine
          int get_size (void);

          // same for copy
          void_t copy (std::string tgt);

          // allow adaptor to obtain instance data (unlocked)
          saga::util::shared_ptr <file_instance_data> get_instance_data (void)
          {
            return idata_;
          }


      }; // class file

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
      class adaptor_0 : public saga::impl::filesystem::file_cpi
      {
        private:
          typedef saga::impl::void_t               void_t;
          typedef saga::impl::filesystem::file     api_t;
          typedef saga::impl::filesystem::file_cpi cpi_t;

          std::string url_;

        public:
          adaptor_0    (void) { std::cout << "adaptor 0 : ctor" << std::endl; } 

          void_t constructor (
              saga::util::shared_ptr <saga::impl::functor <api_t, cpi_t, void_t> > func, 
              saga::util::shared_ptr <api_t>                                       impl, 
              std::string                                                          url) 
          { 
            std::cout << "adaptor 0 : constructor (" << url << ")" << std::endl;
            url_ = url;
            return void_t ();
          } 

          int get_size (
              saga::util::shared_ptr <saga::impl::functor <api_t, cpi_t, int> >    func,
              saga::util::shared_ptr <saga::impl::filesystem::file>                impl)
          {
            std::cout << "adaptor 0 : get_size" << std::endl;
            throw "oops";
          }

          void_t copy (
              saga::util::shared_ptr <saga::impl::functor <api_t, cpi_t, void_t> > func, 
              saga::util::shared_ptr <api_t>                                       impl, 
              std::string                                                          tgt)
          {
            std::cout << "adaptor 0 : copy " << tgt << std::endl;
            throw "oops";
            return void_t ();
          }
      };


      class adaptor_1 : public saga::impl::filesystem::file_cpi
      {
        private:
          typedef saga::impl::void_t                         void_t;
          typedef saga::impl::filesystem::file               api_t;
          typedef saga::impl::filesystem::file_cpi           cpi_t;
          typedef saga::impl::filesystem::file_instance_data idata_t;

          std::string url_;

        public:
          adaptor_1    (void) { std::cout << "adaptor 1 : ctor" << std::endl; } 

          void_t constructor (
              saga::util::shared_ptr <saga::impl::functor <api_t, cpi_t, void_t> > func, 
              saga::util::shared_ptr <api_t>                                       impl, 
              std::string                                                          url) 
          { 
            std::cout << "adaptor 1 : constructor (" << url << ")" << std::endl;
            url_ = url;
            return void_t ();
          } 

          int get_size (
              saga::util::shared_ptr <saga::impl::functor <api_t, cpi_t, int> >    func, 
              saga::util::shared_ptr <api_t>                                       impl)
          {
            std::cout << "adaptor 1 : get_size" << std::endl;
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            struct stat buf;
            (void) stat (idata->url.c_str (), &buf);

            return buf.st_size;
          }

          void_t copy (
              saga::util::shared_ptr <saga::impl::functor <api_t, cpi_t, void_t> > func, 
              saga::util::shared_ptr <api_t>                                       impl, 
              std::string                                                          tgt)
          {
            std::cout << "adaptor 1 : copy " << tgt << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            int res = ::system ((std::string ("cp ") + idata->url + " " + tgt).c_str ());

            if ( res != 0 )
            {
              throw "system command error";
            }

            return void_t ();
          }
      };
    } // namepsace test
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

        RET (CPI::*call_)(
            saga::util::shared_ptr <functor <IMPL, CPI, RET> >,
            saga::util::shared_ptr <IMPL>); 

      public: 
        functor_0 (RET (CPI::*call )(
                saga::util::shared_ptr <functor <IMPL, CPI, RET> >,
                saga::util::shared_ptr <IMPL>)) 
        { 
          call_  = call; 
        }

        RET call_cpi (saga::util::shared_ptr <func_t>  func, 
                      saga::util::shared_ptr <CPI>     cpi, 
                      saga::util::shared_ptr <IMPL>    impl) 
        { 
          return ((*cpi).*(call_)) (func, impl); 
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

        RET (CPI::*call_)(
            saga::util::shared_ptr <functor <IMPL, CPI, RET> >,
            saga::util::shared_ptr <IMPL>, 
            ARG_1); 

        ARG_1 arg_1_;

      public: 
        functor_1 (RET (CPI::*call )(
                saga::util::shared_ptr <functor <IMPL, CPI, RET> >,
                saga::util::shared_ptr <IMPL>, 
                ARG_1), 
            ARG_1 arg_1) 
        { 
          call_  = call; 
          arg_1_ = arg_1; 
        }

        RET call_cpi (saga::util::shared_ptr <func_t> func, 
                      saga::util::shared_ptr <CPI>    cpi, 
                      saga::util::shared_ptr <IMPL>   impl) 
        { 
          return ((*cpi).*(call_)) (func, impl, arg_1_); 
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
        std::vector <saga::util::shared_ptr <saga::impl::filesystem::file_cpi> > file_cpis_;

      public:
        engine (void)
        {
          std::cout << "engine: register all adaptors" << std::endl;
          // create and register adaptor instances
          file_cpis_.push_back (saga::util::shared_ptr <saga::impl::filesystem::file_cpi> (new saga::adaptor::test::adaptor_0));
          file_cpis_.push_back (saga::util::shared_ptr <saga::impl::filesystem::file_cpi> (new saga::adaptor::test::adaptor_1));
        }


        // get_cpi's is used by the API class implementation to get an (ordered)
        // list of adaptors to call
        template <typename CPI_TYPES>
        std::vector <saga::util::shared_ptr <CPI_TYPES> > get_cpis (void)
        {
          // this implementation is very stupid, obviously...
          return file_cpis_;
        }


        //////////////////////////////////////////////////////////////////
        //
        // 
        //
        template <typename IMPL, typename CPI, typename RET>
        RET call (saga::util::shared_ptr <saga::impl::functor <IMPL, CPI, RET> > func, 
                  saga::util::shared_ptr <IMPL> impl)
        {
          // get the matching list of CPIs
          std::vector <saga::util::shared_ptr <CPI> > cpis_ = get_cpis <CPI> ();

          // try one adaptor after the other, until one succeeds.
          for ( unsigned int i = 0; i < cpis_.size (); i++ )
          {
            try
            {
              std::cout << "adaptor " << i << std::endl;

              return func->call_cpi (func, cpis_[i], impl);
            }
            catch ( ... )
            {
              // something went wrong...
              std::cout << "adaptor " << i << " : failed" << std::endl;
            }
          }

          // no adaptor suceeded, throw an exception
          throw "no adaptor suceeded";
        }
    };

  } // namespace impl



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

        int get_size (void)
        {
          return impl_->get_size ();
        }

        void copy (std::string tgt)
        {
          (void) impl_->copy (tgt);
        }
    };
  } // namespace filesystem



  //////////////////////////////////////////////////////////////////////
} // namespace saga

#endif //  SAGA_CPI_4_HPP

