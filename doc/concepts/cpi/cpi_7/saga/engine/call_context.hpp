
#ifndef SAGA_ENGINE_CALLCONTEXT_HPP
#define SAGA_ENGINE_CALLCONTEXT_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/engine/functors.hpp>
#include <saga/engine/cpi_base.hpp>

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
        enum mode 
        {
          Sync       = 0, 
          Async      = 1, 
          Task       = 2, 
        };

        enum state 
        {
          New        = 0, 
          Running    = 1, 
          Done       = 2, 
          Failed     = 3, 
        };


      // FIXME: don't use async::state
      private:
        saga::util::shared_ptr <shareable>    impl_;            // calling object (has session)
        saga::impl::cpi_mode                  cpi_mode_;        // collect, simple, ...
   ///  saga::impl::call_mode                 mode_;            // sync, async, task
        state                                 call_state_;      // new, running, done, failed ...
   ///  saga::impl::call_state                task_state_;      // new, running, done, failed ...
    //  saga::exception                       exception_;       // exception stack collected from adaptors_used_/failed
    //  saga::util::timestamp                 created_;         // created time stamp
    //  saga::util::timestamp                 start_;           // start time stamp
    //  saga::util::timestamp                 stop_;            // stop time stamp
    //  saga::util::timestamp                 duration_;        // time needed for completion
    //  std::string                           func_name_;       // name of method which created functor
    //  std::string                           func_args_;       // for logging, args given to functor
    //  std::vector <saga::util::log::entry>  log_;             // some audit log
    //  std::vector <std::string>             adaptors_used_;   // adaptors which have been used (audit trail)
    //  std::vector <std::string>             adaptors_;        // adaptors to use
    //  std::vector <std::string>             adaptors_skip_;   // adaptors not to use

        saga::util::shared_ptr <functor_base> func_;            // the functor to call

      public:
        call_context (saga::util::shared_ptr <functor_base> func, 
                      saga::util::shared_ptr <shareable>    impl);

        saga::util::shared_ptr <functor_base> get_func (void);
        saga::util::shared_ptr <shareable>    get_impl (void);

        void                   set_state (state s);
        state                  get_state (void);

    /// void                   set_task_state (saga::impl::call_state s);
    /// saga::impl::call_state get_task_state (void);

    /// void                   set_mode       (saga::impl::call_mode  m);
    /// saga::impl::call_mode  get_mode       (void);

        void dump (std::string msg = "");
    };

  } // namespace impl

} // namespace saga

#endif //  SAGA_ENGINE_CALLCONTEXT_HPP

