
#include <saga/util/enums.hpp>

#include <saga/engine/call_context.hpp>
#include <saga/engine/functors.hpp>

namespace saga
{
  namespace impl
  {
    //////////////////////////////////////////////////////////////////
    //
    // container for things to pass around on any method call
    //
    SAGA_UTIL_REGISTER_ENUM_S (call_context::state  , saga__impl__call_context__state, New    , 0);
    SAGA_UTIL_REGISTER_ENUM_S (call_context::state  , saga__impl__call_context__state, Running, 1);
    SAGA_UTIL_REGISTER_ENUM_S (call_context::state  , saga__impl__call_context__state, Done   , 2);
    SAGA_UTIL_REGISTER_ENUM_S (call_context::state  , saga__impl__call_context__state, Failed , 3);


    call_context::call_context (saga::util::shared_ptr <functor_base>          func, 
                                saga::util::shared_ptr <saga::util::shareable> impl)
      : func_        (func)
      , impl_        (impl)
      , cpi_mode_    (Simple)
      , call_state_  (New)
    {
      SAGA_UTIL_STACKTRACE ();
    }

    saga::util::shared_ptr <functor_base>           call_context::get_func (void)  { return func_;      } 
    saga::util::shared_ptr <saga::util::shareable>  call_context::get_impl (void)  { return impl_;      } 

    void                   call_context::set_state (call_context::state s)         { call_state_ = s;   }
    call_context::state    call_context::get_state (void)                          { return call_state_;}

    void call_context::dump (std::string msg)
    {
      // FIXME: to_key can throw
      LOGSTR (DEBUG, "call_context dump")
        <<   "call_context (" << this << ") : " << msg << std::endl
        <<   "    cpi  mode   : " << saga::util::saga_enum_to_key <saga::impl::cpi_mode>            (cpi_mode_  ) << std::endl
        <<   "    call state  : " << saga::util::saga_enum_to_key <saga::impl::call_context::state> (call_state_) << std::endl;
    //  <<   "    func name   : " << func_name_ << std::endl
    //  <<   "    func args   : " << func_args_ << std::endl;
      impl_.dump         ("    IMPL_       : ");
      impl_->dump        ();
      func_.dump         ("    func_       : ");
      func_->dump        ();
    }

  } // namespace impl

} // namespace saga


