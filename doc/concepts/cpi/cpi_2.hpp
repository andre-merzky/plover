
#ifndef SAGA_CPI_2_HPP
#define SAGA_CPI_2_HPP

#include <vector>
#include <iostream>

#include <saga/util/shared_ptr.hpp>

// In the cpi_1 example, each class instance maintains its own set of shared
// pointers to the cpi classes, and in fact populates those shared pointers
// itself.  We have thus a complete set of CPI's for each object instance.  That
// is (a) wastefule, and (b) convulates the code, as we'd rather want cpi
// instance management close to the adaptor management -- in the engine.
//

namespace saga
{
  //////////////////////////////////////////////////////////////////////
  //
  // saga::file cpi
  //
  namespace cpi
  {
    class file
    {
      public:
        virtual int get_size (void)
        {
          throw "get_size : NotImplemented";
        }
    };
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

      // get_size () seems relatively complex, as it does it's own cpi
      // management, and that code will look very much the same for each and
      // every function call.  Also, the file class will need to implement the
      // async method calls, etc.  See cpi_3.h how that is being simplified.

      int get_size (void)
      {
        // get the available CPIs from the engine
        std::vector <saga::util::shared_ptr <saga::cpi::file> > file_cpis_ 
              = engine_->get_cpis <saga::cpi::file> ();

        // try one adaptor after the other, until one succeeds.
        for ( unsigned int i = 0; i < file_cpis_.size (); i++ )
        {
          try
          {
            std::cout << "adaptor " << i << std::endl;
            return file_cpis_[i]->get_size ();
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
}

#endif //  SAGA_CPI_2_HPP

