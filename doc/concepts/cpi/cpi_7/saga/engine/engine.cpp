
#include <saga/util/enums.hpp>

#include <saga/adaptor/test/filesystem_adaptor_0.hpp>
#include <saga/adaptor/test/filesystem_adaptor_1.hpp>
#include <saga/adaptor/test/async_adaptor_0.hpp>

#include "engine.hpp"

namespace saga
{
  namespace impl
  {
    saga::util::shared_ptr <engine> & engine::the_engine (void)
    {
      static saga::util::shared_ptr <engine> the_engine_ (new engine ());
      return the_engine_;
    }

    engine::engine (void)
    {
      SAGA_UTIL_STACKTRACE ();

      LOGSTR (INFO, "engine") << "engine: register adaptors";

      // create and register adaptor instances
      cpis_.push_back (open_adaptor <saga::adaptor::test::file_adaptor_0>  ());
      cpis_.push_back (open_adaptor <saga::adaptor::test::file_adaptor_1>  ());
      cpis_.push_back (open_adaptor <saga::adaptor::test::dir_adaptor_0>   ());
      cpis_.push_back (open_adaptor <saga::adaptor::test::dir_adaptor_1>   ());
      cpis_.push_back (open_adaptor <saga::adaptor::test::async_adaptor_0> ());
    }

    void engine::dump (std::string msg)
    {
      LOGSTR (DEBUG, "engine dump") 
        << "engine (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;

      for ( unsigned int i = 0; i < cpis_.size (); i++ )
      {
        cpis_[i].dump();
      }
    }

  } // namespace impl

} // namespace saga

