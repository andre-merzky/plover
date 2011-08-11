
#ifndef SAGA_CPI_4_HPP
#define SAGA_CPI_4_HPP

#include <map>
#include <vector>

#include <saga/util/shared_ptr.hpp>

// In the cpi_3 example, the engine took over the adaptor loading and
// management.  The saga::impl classes should, however, have some amount of
// interaction with the adaptor list.  For one, it should maintain object state
// information between subsequent calls, but also it should be able to re-order
// adaptors, and for example prefer successful ones on re-try.
//
// This example shows how those features can be achieved.
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

  namespace util
  {
    class void_t;
  }
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
    template <typename CPI, typename RET>                 class functor;
    template <typename CPI, typename RET>                 class functor_0;
    template <typename CPI, typename RET, typename ARG_1> class functor_1;
    class engine;
  }
  namespace filesystem
  {
    class file;
  }



  namespace util
  {
    //////////////////////////////////////////////////////////////////
    // void is not a C++ type, really.  This helped struct allows us 
    // to use void_t as template parameter, instead of void itself.  
    class void_t 
    { 
      private: 
        void_t (void) {};  // never allow to consruct instances
    };
  } // namespace util

  namespace impl
  {
    //////////////////////////////////////////////////////////////////
    //
    // the filesystem implementation
    //
    namespace filesystem
    {
      class file_cpi
      {
        public:
          virtual void constructor (saga::impl::filesystem::file * impl, std::string url) { throw "constructor: NotImplemented"; } 
          virtual int  get_size    (saga::impl::filesystem::file * impl)                  { throw "get_size   : NotImplemented"; } 
          virtual void copy        (saga::impl::filesystem::file * impl, std::string tgt) { throw "copy       : NotImplemented"; }
      };

      // saga api class
      class file_instance_data
      {
        private:
          saga::util::mutex mtx_;
          std::map <std::string, void*> adata_;
          // saga::session s_;

        public:
          std::string url;
          size_t      pos;

          saga::util::mutex * get_mutex (void)
          {
            return &mtx_;
          }
      };


      class file
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
          file (std::string url)               // FIXME: the file impl and pimpl object hierarchies are ignored
            : engine_ (new saga::impl::engine) // FIXME: no need to create a new engine - get it from the session!
            , idata_  (new file_instance_data)
          {
            saga::util::scoped_lock sl (idata_->get_mutex ());
            idata_->url = url;
            idata_->pos = 0;

            saga::impl::functor_1 <saga::impl::filesystem::file_cpi,
                                   saga::util::void_t, 
                                   std::string>  
                            func (&saga::impl::filesystem::file_cpi::constructor, url);
            engine_->call         <saga::impl::filesystem::file_cpi, 
                                   saga::impl::filesystem::file, 
                                   saga::util::void_t> (func, this);
          }

          saga::util::shared_ptr <file_instance_data> get_instance_data (void)
          {
            return idata_;
          }


          // instead of get_size calling each cpi individually, the call invocation
          // is passed off to the engine
          int get_size (void)
          {
            // create a functor which hold the cpi class' get_size() function
            // pointer.  The second templ parameter is the functions return type
            saga::impl::functor_0  <saga::impl::filesystem::file_cpi, int> func (&saga::impl::filesystem::file_cpi::get_size);

            // the functor is given to the engine, so it can be used to call that
            // function on some cpi
            return engine_->call   <saga::impl::filesystem::file_cpi,
                                    saga::impl::filesystem::file, 
                                    int> (func, this);
          }


          // same for copy
          void copy (std::string tgt)
          {
            saga::impl::functor_1 <saga::impl::filesystem::file_cpi, 
                                   saga::util::void_t, 
                                   std::string> 
                         func (&saga::impl::filesystem::file_cpi::copy, tgt);

            // as before, the functor is given to the engine, so it can be used 
            // to call that function on some cpi - but also
            engine_->call_1 <saga::impl::filesystem::file_cpi, 
                             saga::impl::filesystem::file, 
                             saga::util::void_t, 
                             std::string> (func, this, tgt);
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
          std::string url_;

        public:
          adaptor_0    (void) { std::cout << "adaptor 0 : ctor" << std::endl; } 

          void constructor (saga::impl::filesystem::file * impl, std::string url) 
          { 
            std::cout << "adaptor 0 : constructor (" << url << ")" << std::endl;
            url_ = url;
          } 

          int get_size (saga::impl::filesystem::file * impl)
          {
            std::cout << "adaptor 0 : get_size" << std::endl;
            throw "oops";
            return 1024;
          }

          void copy (saga::impl::filesystem::file * impl, 
                     std::string                    tgt)
          {
            std::cout << "adaptor 0 : copy " << tgt << std::endl;
            throw "oops";
            return;
          }
      };


      class adaptor_1 : public saga::impl::filesystem::file_cpi
      {
        private:
          std::string url_;

        public:
          adaptor_1    (void) { std::cout << "adaptor 1 : ctor" << std::endl; } 

          void constructor (saga::impl::filesystem::file * impl, std::string url) 
          { 
            std::cout << "adaptor 1 : constructor (" << url << ")" << std::endl;
            url_ = url;
          } 

          int get_size (saga::impl::filesystem::file * impl)
          {
            std::cout << "adaptor 1 : get_size" << std::endl;
            return 2048;
          }

          void copy (saga::impl::filesystem::file * impl,
                     std::string                    tgt)
          {
            std::cout << "adaptor 1 : copy " << tgt << std::endl;

            saga::util::shared_ptr <saga::impl::filesystem::file_instance_data> idata = impl->get_instance_data ();
            // ::system (copy idata->url tgt);
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
    //
    // father of all non-void functors
    //
    template <typename CPI, typename RET>
    class functor 
    {
      public:
        virtual ~functor (void) = 0;
    };

    //////////////////////////////////////////////////////////////////
    //
    // father of all void functors
    //
    template <typename CPI>
    class functor <CPI, saga::util::void_t>
    {
      public:
        virtual ~functor (void) = 0;
    };

    // non-void ret type, 0 args
    template <typename CPI, typename RET> 
    class functor_0 : public functor <CPI, RET>
    {
      private:           
        RET (CPI::*call_)(saga::impl::filesystem::file *); 

      public: 
        functor_0 (RET (CPI::*call )(saga::impl::filesystem::file *)) 
        { 
          call_ = call; 
        } 

        RET call_cpi (saga::util::shared_ptr <CPI> cpi, saga::impl::filesystem::file * impl) 
        { 
          return ((*cpi).*(call_)) (impl); 
        }
    };

    //////////////////////////////////////////////////////////////////
    // void ret type, 0 args
    template <typename CPI> 
    class functor_0 <CPI, saga::util::void_t>  : public functor <CPI, saga::util::void_t>
    {
      private:           
        void (CPI::*call_)(saga::impl::filesystem::file *); 

      public: 
        functor_0 (void (CPI::*call )(saga::impl::filesystem::file *)) 
        { 
          call_ = call; 
        }

        void call_cpi (saga::util::shared_ptr <CPI>   cpi, 
                       saga::impl::filesystem::file * impl) 
        { 
          ((*cpi).*(call_)) (impl); 
        }
    };


    //////////////////////////////////////////////////////////////////
    // non-void ret type, 1 args
    template <typename CPI, 
              typename RET, 
              typename ARG_1> 
    class functor_1 : public functor <CPI, RET>
    {
      private:           
        RET (CPI::*call_)(saga::impl::filesystem::file *, ARG_1); 
        ARG_1 arg_1_;

      public: 
        functor_1 (RET (CPI::*call )(saga::impl::filesystem::file *, ARG_1), ARG_1 arg_1) 
        { 
          call_  = call; 
          arg_1_ = arg_1; 
        }

        RET call_cpi (saga::util::shared_ptr <CPI>   cpi, 
                      saga::impl::filesystem::file * impl) 
        { 
          return ((*cpi).*(call_)) (impl, arg_1_); 
        }
    };

    //////////////////////////////////////////////////////////////////
    // void ret type, 1 args
    template <typename CPI, 
              typename ARG_1> 
    class functor_1 <CPI, saga::util::void_t, ARG_1> : public functor <CPI, saga::util::void_t>
    {
      private:           
        void (CPI::*call_)(saga::impl::filesystem::file *, ARG_1); 
        ARG_1 arg_1_;

      public: 
        functor_1 (void (CPI::*call )(saga::impl::filesystem::file *, ARG_1), ARG_1 arg_1) 
        { 
          call_  = call; 
          arg_1_ = arg_1; 
        }

        void call_cpi (saga::util::shared_ptr <CPI>   cpi, 
                       saga::impl::filesystem::file * impl) 
        { 
          ((*cpi).*(call_)) (impl, arg_1_); 
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
    class engine
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
        // non-void ret type
        //
        template <typename CPI, typename IMPL, typename RET>
        RET call (saga::impl::functor <CPI, RET> func, 
                  IMPL * impl)
        {
          // get the matching list of CPIs
          std::vector <saga::util::shared_ptr <CPI> > cpis_ = get_cpis <CPI> ();

          // try one adaptor after the other, until one succeeds.
          for ( unsigned int i = 0; i < cpis_.size (); i++ )
          {
            try
            {
              std::cout << "adaptor " << i << std::endl;

              RET out = func.call_cpi (cpis_[i], impl);

              return out;
            }
            catch ( ... )
            {
              // something went wrong...
              std::cout << "adaptor " << i << " : failed" << std::endl;
            }
          }

          // no adaptor suceeded, throw an exception
          throw "get_size: no adaptor suceeded";
        }

        //////////////////////////////////////////////////////////////////
        //
        // void ret type
        //
        template <typename CPI, typename IMPL>
        void call (saga::impl::functor <CPI, saga::util::void_t> func, 
                   IMPL * impl) 
             <CPI, IMPL, saga::util::void_t>
        {
          // get the matching list of CPIs
          std::vector <saga::util::shared_ptr <CPI> > cpis_ = get_cpis <CPI> ();

          // try one adaptor after the other, until one succeeds.
          for ( unsigned int i = 0; i < cpis_.size (); i++ )
          {
            try
            {
              std::cout << "adaptor " << i << std::endl;

              func.call_cpi (cpis_[i], impl);

              return;
            }
            catch ( ... )
            {
              // something went wrong...
              std::cout << "adaptor " << i << " : failed" << std::endl;
            }
          }

          // no adaptor suceeded, throw an exception
          throw "get_size: no adaptor suceeded";
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
          : impl_ (new saga::impl::filesystem::file (url))
        {
        }

        int get_size (void)
        {
          return impl_->get_size ();
        }

        void copy (std::string tgt)
        {
          impl_->copy (tgt);
        }
    };
  } // namespace filesystem



  //////////////////////////////////////////////////////////////////////
} // namespace saga

#endif //  SAGA_CPI_4_HPP

