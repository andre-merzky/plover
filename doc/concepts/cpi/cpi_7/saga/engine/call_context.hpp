
#ifndef SAGA_ENGINE_CALLCONTEXT_HPP
#define SAGA_ENGINE_CALLCONTEXT_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/api/async/state.hpp>

#include <saga/engine/impl_base.hpp>
#include <saga/engine/func.hpp>

//////////////////////////////////////////////////////////////////////
//
// The call_context is *the* central data structure: as soon as API calls reach
// the impl level, they are packed into call_contexts (cc), and all further
// operations are done on those cc's.  The cc is thus representing the function
// call and its state.
//
// The motivation for this approach is manyfold:
//
//   - state of asynchronous calls can be kept close to the call itself,
//   - the cc supports auditing, benchmarking and debugging of function calls,
//   - the cc is easy to copy, so that multuple adaptors can, in principle,
//     operate simultaneously on copies (think service discovery) 
//   - the cc is easy to transform, so that filter adaptors can add additional
//     semantics to the function call (think transactions, or adaptor chains)
//
// The cc splits the function call into two parts: a function pointer into the
// cpi classes, and a container for the return value of the call.  That
// separation FIXME: cont
//  
//

namespace saga
{
  namespace impl
  {
    //////////////////////////////////////////////////////////////////
    //
    // container for things to pass around on any method call
    //
    class call_context : public saga::util::shareable
    {
      public:
        // We re-use the API-level task mode, so we do not define it here.
        // Alas, C++ does not define a good way to do something like this:
        // typedef saga::async::mode saga::async::mode;

        enum policy 
        {
          Any        = 0,  // any adaptor(s) can act, allows filter
          Bound      = 1,  // use same adaptor as before
          Collect    = 2,  // results are combined over all adaptors
        };

        // FIXME: we would like to inherit the saga::async enums, but that is not
        // possible in C++ :-(
        //
        // enum state 
        // {
        //   New        = 0, 
        //   Running    = 1, 
        //   Done       = 2, 
        //   Failed     = 3   // Needed?
        // };

        // enum mode 
        // {
        //   Sync       = 0, 
        //   Async      = 1, 
        //   Task       = 2
        // };


      // FIXME: don't use async::state, but call_context::state (which might
      // have same states though...)
      private:
        saga::util::shared_ptr <impl_base>    impl_;              // calling object (has session)
        saga::util::shared_ptr <func_base>    func_;              // func to be called on object
        saga::async::mode                     mode_;              // call mode   - sync, async, task
        saga::async::state                    state_;             // call state  - new, running, done, failed ...
        policy                                policy_;            // call policy - any, bound, collect, ...

    //  saga::exception                       exception_;         // exception stack collected from adaptors_used_/failed
    //  saga::util::timestamp                 created_;           // created time stamp
    //  saga::util::timestamp                 start_;             // start time stamp
    //  saga::util::timestamp                 stop_;              // stop time stamp
    //  saga::util::timestamp                 duration_;          // time needed for completion ? 
    //  std::vector <saga::util::log::entry>  log_;               // some audit log
    //  std::vector <std::string>             cpis_;              // adaptor types to use
    //  std::vector <std::string>             adaptors_;          // adaptors to use
    //  std::vector <std::string>             adaptors_skip_;     // adaptors not to use
    //  std::vector <std::string>             adaptors_used_;     // adaptors which have been used (audit trail)

      public:
        call_context (saga::util::shared_ptr <impl_base> impl, 
                      saga::util::shared_ptr <func_base> func);

        saga::util::shared_ptr <impl_base> get_impl   (void);
        saga::util::shared_ptr <func_base> get_func   (void);

        void                               set_mode   (saga::async::mode   m);
        saga::async::mode                  get_mode   (void);

        void                               set_state  (saga::async::state  s);
        saga::async::state                 get_state  (void);

        void   set_policy (policy p);
        policy get_policy (void);

        void dump (std::string msg = "");
    };

  } // namespace impl

} // namespace saga

#endif //  SAGA_ENGINE_CALLCONTEXT_HPP

