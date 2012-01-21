
#ifndef SAGA_ENGINE_CALLCONTEXT_HPP
#define SAGA_ENGINE_CALLCONTEXT_HPP

#include <string>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/api/async/state.hpp>

#include <saga/engine/cpi_base.hpp>
#include <saga/engine/result_types.hpp>

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
        // typedef saga::async::mode saga::impl::call_context::mode;

        enum policy 
        {
          Any        = 0,  // any adaptor(s) can act, allows filter
          Bound      = 1,  // use same adaptor as before
          Collect    = 2,  // results are combined over all adaptors
        };

        enum state 
        {
          New        = 0, 
          Running    = 1, 
          Done       = 2, 
          Failed     = 3   // Needed?
        };


      // FIXME: don't use async::state
      private:
        saga::util::shared_ptr <shareable>    impl_;            // calling object (has session)
        saga::async::mode                     mode_;            // sync, async, task
        state                                 state_;           // new, running, done, failed ...
        policy                                policy_;          // any, bound, collect, ...
   ///  saga::impl::call_state                task_state_;      // new, running, done, failed ...
    //  saga::exception                       exception_;       // exception stack collected from adaptors_used_/failed
    //  saga::util::timestamp                 created_;         // created time stamp
    //  saga::util::timestamp                 start_;           // start time stamp
    //  saga::util::timestamp                 stop_;            // stop time stamp
    //  saga::util::timestamp                 duration_;        // time needed for completion
    //  std::vector <saga::util::log::entry>  log_;             // some audit log
    //  std::vector <std::string>             adaptors_used_;   // adaptors which have been used (audit trail)
    //  std::vector <std::string>             adaptors_;        // adaptors to use
    //  std::vector <std::string>             adaptors_skip_;   // adaptors not to use

        saga::util::shared_ptr <saga::impl::result_t> result_;  // container for function call result

      public:
        call_context (saga::util::shared_ptr <shareable>    impl);

        saga::util::shared_ptr <shareable>    get_impl (void);

        void                set_mode   (saga::async::mode   m);
        saga::async::mode   get_mode   (void);

        void   set_state  (state  s);
        state  get_state  (void);

        void   set_policy (policy p);
        policy get_policy (void);

        void dump (std::string msg = "");


        template <typename T>
        void set_result (T res)
        {
          // FIXME: make sure this is called only once, or, at least, always
          // called with the same type.  We might want to do that on
          // result_t_detail level though...

          SAGA_UTIL_STACKTRACE ();
          result_ = saga::util::shared_ptr <saga::impl::result_t_detail_ <T> > (new saga::impl::result_t_detail_ <T> ());
          result_->set <T> (res);
        }

        template <typename T>
        bool has_result_type  (void) 
        {
          SAGA_UTIL_STACKTRACE ();

          return result_.is_a <saga::impl::result_t_detail_ <T> > ();
        }


        template <typename T>
        T get_result  (void) 
        {
          SAGA_UTIL_STACKTRACE ();

          if ( ! has_result_type <T> () )
          {
            throw "Incorrect result type requested";
          }

          return result_->get <T> ();
        }

        saga::util::shared_ptr <saga::impl::result_t> get_result  (void);
    };

  } // namespace impl

} // namespace saga

#endif //  SAGA_ENGINE_CALLCONTEXT_HPP

