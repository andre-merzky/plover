
#include "engine.hpp"

static saga::util::shared_ptr <saga::impl::engine> the_engine (new saga::impl::engine);

namespace saga
{
  namespace impl
  {
    impl_base::impl_base (void)
      : engine_ (the_engine) // FIXME: no need to create a new engine - get it from the session!
    {
    }

    void impl_base::dump (std::string msg)
    {
      LOGSTR (DEBUG, "impl_base dump") << "impl_base (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) 
                << " : " << msg  << std::endl;
      engine_.dump ("    engine_     : ");
      engine_->dump();
    }

  } // namespace impl

} // namespace saga

