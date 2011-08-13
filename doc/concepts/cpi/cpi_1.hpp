
#ifndef SAGA_CPI_1_HPP
#define SAGA_CPI_1_HPP

#include <vector>
#include <iostream>

#include <saga/util/shared_ptr.hpp>

// the cpi classes represent the adaptor base classes.  CPI stands for
// 'capability provider interface', and it basically reflects the api
// (application programming interface), but adds some additional method 
// parameters and utility functions, to support adaptor development.
//
// The SAGA engine will not directly handle the adaptor classes, but instead
// will treat them all as CPI classes (which they inherit from, so this is
// straight forward).  We need to make the cpi classes virtual though, in order
// to call the correct adaptor methods, at the end.  Also, we need to ensure that
// only the engine can access the cpi's management methods - so we make those
// private, and be-friend the engine
//
// For each SAGA api object, the engine needs to maintain a list of potential
// cpi instances.  That list may be ordered by the engine after some criteria.
// On API calls, the engine tries one cpi after the other, until one succeeds.  
// For some calls, the engine will need to try *all* adaptors, and collate the
// results 
//
//   saga::util::data_append <std::list <saga::url> > (global_list, adapotor _list);
// 
// also, the engine needs to be able to wrap all cpi functions into threads, to
// implement async operations.  That in particular changes the api signatures --
// results are returned as references.  Those refs will always be the first
// parameter, for consistency, and to potentially allow for default parameters.
//

namespace saga
{
  //////////////////////////////////////////////////////////////////////
  //
  // saga::file cpi
  //
  namespace cpi
  {
    class file : public saga::util::shareable
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
  // the SAGA API class implementation, which forwards all calls to 
  // one of the known CPI instances.
  //
  class file : public saga::util::shareable
  {
    private:
      std::vector <saga::util::shared_ptr <saga::cpi::file> > cpis_;

    public:
      file (void)
      {
        // create and register adaptor instances
        cpis_.push_back (saga::util::shared_ptr <saga::cpi::file> (new saga::adaptor::adaptor_0 ()));
        cpis_.push_back (saga::util::shared_ptr <saga::cpi::file> (new saga::adaptor::adaptor_1 ()));
      }

      int get_size (void)
      {
        // try one adaptor after the other, until one succeeds.
        for ( unsigned int i = 0; i < cpis_.size (); i++ )
        {
          try
          {
            std::cout << "adaptor " << i << std::endl;
            return cpis_[i]->get_size ();
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

#endif //  SAGA_CPI_1_HPP

