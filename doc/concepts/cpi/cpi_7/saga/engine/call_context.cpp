
#include <saga/util/enums.hpp>

#include <saga/engine/call_context.hpp>

namespace saga
{
  namespace impl
  {
    SAGA_UTIL_REGISTER_ENUM_S (call_context::policy, saga__impl__call_context__policy, Any       , 0);
    SAGA_UTIL_REGISTER_ENUM_S (call_context::policy, saga__impl__call_context__policy, Bound     , 1);
    SAGA_UTIL_REGISTER_ENUM_S (call_context::policy, saga__impl__call_context__policy, Collect   , 3);

 // SAGA_UTIL_REGISTER_ENUM_S (call_context::state , saga__impl__call_context__state , New       , 0);
 // SAGA_UTIL_REGISTER_ENUM_S (call_context::state , saga__impl__call_context__state , Running   , 1);
 // SAGA_UTIL_REGISTER_ENUM_S (call_context::state , saga__impl__call_context__state , Done      , 2);
 // SAGA_UTIL_REGISTER_ENUM_S (call_context::state , saga__impl__call_context__state , Failed    , 3);


    //////////////////////////////////////////////////////////////////
    //
    // container for things to pass around on any method call
    //
    call_context::call_context (saga::util::shared_ptr <impl_base> impl,
                                saga::util::shared_ptr <func_base> func)
      : impl_      (impl)
      , func_      (func)
      , state_     (saga::async::New)
      , mode_      (saga::async::Sync)
      , policy_    (Any)
    {
      SAGA_UTIL_STACKTRACE ();
    }

    saga::util::shared_ptr <impl_base> call_context::get_impl (void)      { return impl_;   } 
    saga::util::shared_ptr <func_base> call_context::get_func (void)      { return func_;   } 

    void                 call_context::set_mode   (saga::async::mode m)   { mode_ = m;      }
    saga::async::mode    call_context::get_mode   (void)                  { return mode_;   }

    void                 call_context::set_state  (saga::async::state  s) { state_ = s;     }
    saga::async::state   call_context::get_state  (void)                  { return state_;  }

    void                 call_context::set_policy (policy p)              { policy_ = p;    }
    call_context::policy call_context::get_policy (void)                  { return policy_; }


    void call_context::dump (std::string msg)
    {
      // FIXME: to_key() can throw
      LOGSTR (DEBUG, "call_context dump")
                  << "call_context (" << this << ") : " << msg << std::endl
                  << "    mode   : " << saga::util::saga_enum_to_key <saga::async::mode>                (mode_  ) << std::endl
                  << "    state  : " << saga::util::saga_enum_to_key <saga::async::state>               (state_ ) << std::endl
                  << "    policy : " << saga::util::saga_enum_to_key <saga::impl::call_context::policy> (policy_) << std::endl;
      impl_.dump    ("    impl_  : ");
      impl_->dump   ();
      func_.dump    ("    func_  : ");
      func_->dump   ();
    }

  } // namespace impl

} // namespace saga


