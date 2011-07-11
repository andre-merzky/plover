
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
// to at the end call the correct adaptor methods.  Also, we need to ensure that
// only the engine can access the cpi's management methods - so we make those
// private, and be-friend the engine
//
// for each SAGA api object, the engine needs to maintain a list of potential
// cpi instances.  That list may be ordered by the engine after some creteria.
// On API calls, the engine tries one cpi after the other, until one succeeds.  
// For some calls, the engine will need to try *all* adaptors, and collate the
// results 
//
//   saga::util::data_append <std::list <saga::url> > (global_list, adapotor _list);
// 
// also, the engine needs to be able to wrap all cpi functions into threads, to
// implement async operations.  That in particular changes the api signatures --
// the results are returned in a reference.  That will always be the first
// parameter, for consistency, and to potentially allow for default parameter
// values
//
namespace saga
{
  //////////////////////////////////////////////////////////////////////
  //
  //
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
  //
  //
  namespace adaptor
  {
    class adaptor_1 : public saga::cpi::file
    {
      public:
        int get_size (void)
        {
          return 1024;
        }
    };


    class adaptor_2 : public saga::cpi::file
    {
      public:
        int get_size (void)
        {
          return 2048;
        }
    };
  }

  //////////////////////////////////////////////////////////////////////
  //
  //
  //
  class file
  {
    private:
      std::vector <saga::util::shared_ptr <saga::cpi::file> > cpis_;

    public:
      file (void)
      {
        cpis_.push_back (saga::util::shared_ptr <saga::cpi::file> (new saga::adaptor::adaptor_1 ()));
        cpis_.push_back (saga::util::shared_ptr <saga::cpi::file> (new saga::adaptor::adaptor_2 ()));
      }

      int get_size (void)
      {
        return cpis_[1]->get_size ();
      }
  };

  //////////////////////////////////////////////////////////////////////
  //
  //
}

#endif //  SAGA_CPI_1_HPP

