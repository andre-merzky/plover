
#ifndef SAGA_CPI_3_HPP
#define SAGA_CPI_3_HPP

#include <vector>
#include <iostream>
#include <functional>

#include <saga/util/shared_ptr.hpp>

// In the cpi_2 example, each class had to repeatedly implement it's own cpi
// invocation, which leads to lots or repeated code, and to relatively complex
// API classes.  This file documents how that functionality is moved back into
// the engine.

namespace saga
{
  //////////////////////////////////////////////////////////////////////
  //
  // saga::file cpi
  //
  namespace cpi
  {
    //////////////////////////////////////////////////////////////////////
    //
    // our file adaptor base class
    //
    class file
    {
      public:
        virtual int get_size (void)
        {
          throw "get_size : NotImplemented";
        }
    };

    //////////////////////////////////////////////////////////////////////
    //
    // functor: stores a cpi function pointer and call arguments, and 
    // calls it on a given cpi
    //
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

      //////////////////////////////////////////////////////////////////
      // non-void ret type, 0 args
      template <typename CPI, typename RET> 
      class functor_0 
      {
        private:           
          RET (CPI::*call_)(void); 
        
        public: 
          functor_0 (RET (CPI::*call )(void)) 
          { 
            call_ = call; 
          } 

          RET call_cpi (saga::util::shared_ptr <CPI> cpi) 
          { 
            return ((*cpi).*(call_)) (); 
          }
      };

      //////////////////////////////////////////////////////////////////
      // void ret type, 0 args
      template <typename CPI> 
      class functor_0 <CPI, void_t> 
      {
        private:           
          void (CPI::*call_)(void); 
        
        public: 
          functor_0 (void (CPI::*call )(void)) 
          { 
            call_ = call; 
          }

          void call_cpi (saga::util::shared_ptr <CPI> cpi) 
          { 
            ((*cpi).*(call_)) (); 
          }
      };


      //////////////////////////////////////////////////////////////////
      // non-void ret type, 1 args
      template <typename CPI, 
                typename RET, 
                typename ARG_1> 
      class functor_1 
      {
        private:           
          RET (CPI::*call_)(ARG_1); 
          ARG_1 arg_1_;
        
        public: 
          functor_1 (RET (CPI::*call )(ARG_1), ARG_1 arg_1) 
          { 
            call_  = call; 
            arg_1_ = arg_1; 
          }

          RET call_cpi (saga::util::shared_ptr <CPI> cpi) 
          { 
            return ((*cpi).*(call_)) (arg_1_); 
          }
      };

      //////////////////////////////////////////////////////////////////
      // void ret type, 1 args
      template <typename CPI, 
                typename ARG_1> 
      class functor_1 <CPI, void_t, ARG_1>
      {
        private:           
          void (CPI::*call_)(ARG_1); 
          ARG_1 arg_1_;
        
        public: 
          functor_1 (void (CPI::*call )(ARG_1), ARG_1 arg_1) 
          { 
            call_  = call; 
            arg_1_ = arg_1; 
          }

          void call_cpi (saga::util::shared_ptr <CPI> cpi) 
          { 
            ((*cpi).*(call_)) (arg_1_); 
          }
      };


      //////////////////////////////////////////////////////////////////
      //
      // FIXME: functor versions for more args follow here....
      //
    }
  }

  //////////////////////////////////////////////////////////////////////
  //
  // two independent CPI implementations, aka adaptors.
  //
  namespace adaptor
  {
    class adaptor_0 : public saga::cpi::file
    {
      public:
        adaptor_0    (void) { std::cout << "adaptor 0 : ctor" << std::endl; } 
        int get_size (void)
        {
          std::cout << "adaptor 0 : get_size" << std::endl;
          throw "oops";
          return 1024;
        }
    };


    class adaptor_1 : public saga::cpi::file
    {
      public:
        adaptor_1    (void) { std::cout << "adaptor 1 : ctor" << std::endl; } 
        int get_size (void)
        {
          std::cout << "adaptor 1 : get_size" << std::endl;
          return 2048;
        }
    };
  }


  // for now, we only consider the impl side of things, and ignore the pimple
  // part
  namespace impl
  {
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
        std::vector <saga::util::shared_ptr <saga::cpi::file> > file_cpis_;

      public:
        engine (void)
        {
          std::cout << "engine: register all adaptors" << std::endl;
          // create and register adaptor instances
          file_cpis_.push_back (saga::util::shared_ptr <saga::cpi::file> (new saga::adaptor::adaptor_0 ()));
          file_cpis_.push_back (saga::util::shared_ptr <saga::cpi::file> (new saga::adaptor::adaptor_1 ()));
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
        // non-void ret type, 0 arguments
        //
        template <typename CPI, typename RET>
          RET call_0 (saga::cpi::util::functor_0 <CPI, RET> func)
          {
            // get the matching list of CPIs
            std::vector <saga::util::shared_ptr <CPI> > cpis_ = get_cpis <CPI> ();

            // try one adaptor after the other, until one succeeds.
            for ( unsigned int i = 0; i < cpis_.size (); i++ )
            {
              try
              {
                std::cout << "adaptor " << i << std::endl;

                RET out = func.call_cpi (cpis_[i]);

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
    };

    //////////////////////////////////////////////////////////////////////
    //
    // the SAGA API class implementation, which forwards all calls to 
    // one of the known CPI instances.
    //
    class file
    {
      private:
        saga::util::shared_ptr <engine> engine_;

      public:
        file (void)
          : engine_ (new engine)
        {
        }

        // instead of get_size calling each cpi individually, the call invocation
        // is passed off to the engine
        int get_size (void)
        {
          // create a functor which hold the cpi class' get_size() function
          // pointer.  The second templ parameter is the functions return type
          saga::cpi::util::functor_0 <saga::cpi::file, int> func (&saga::cpi::file::get_size);

          // the functor is given to the engine, so it can be used to call that
          // function on some cpi
          return engine_->call_0 (func);
        }
    };


  } // namespace impl


  //////////////////////////////////////////////////////////////////////
}

#endif //  SAGA_CPI_3_HPP

