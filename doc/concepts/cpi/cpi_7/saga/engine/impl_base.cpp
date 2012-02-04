
#include <saga/engine/engine.hpp>
#include <saga/engine/impl_base.hpp>


namespace saga
{
  namespace impl
  {
    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all IMPLs
    //
    impl_base::impl_base (void) // FIXME: add explicit engine parameter
  /// : engine_ (saga::impl::the_engine::get_singleton ()) // FIXME: no need to create a new engine - get it from the impl_base ctor!
      : engine_ (saga::impl::engine::the_engine ())
    {
    }

    impl_base::~impl_base (void) 
    {
    }

    saga::util::shared_ptr <saga::impl::engine> impl_base::get_engine (void) 
    { 
      return engine_; 
    }

    void impl_base::dump (std::string msg)
    {
      LOGSTR (DEBUG, "impl_base dump") << "impl_base (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) 
                << " : " << msg  << std::endl;
    }

  } // namespace impl

} // namespace saga

