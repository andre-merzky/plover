
#include <string.h>
#include <errno.h>

#include <saga/api/async/task.hpp>
#include <saga/api/async/state.hpp>

// FIXME: away it goes!
#include <saga/cpi/filesystem/file.hpp>

#include "async_adaptor_0.hpp"

namespace saga
{
  namespace adaptor
  {
    namespace test // test adaptprs
    {
      void * async_adaptor_0::threaded_cc (void * t_cc_sp)
      {
        saga::util::shared_ptr <saga::impl::call_context> t_cc; 
        // saga::util::shared_ptr <api_t>                    impl;

        try
        {
          LOGSTR (INFO, "async_adaptor_0 threaded_cc") << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;

          SAGA_UTIL_STACKTRACE ();

          saga::util::shared_ptr <saga::impl::call_context> * t_cc_tmp
            = static_cast <saga::util::shared_ptr <saga::impl::call_context> *> (t_cc_sp);

          t_cc  = t_cc_tmp->get_shared_ptr (); 

          // set task zu Running
          t_cc->set_state (saga::async::Running);

          // signal the main thread that init is done.
          /// t_cc->unlock (); // FIXME

          LOGSTR (INFO, "async_adaptor_0 ctor") << "async adaptor 0 : constructor ()" << std::endl;
          LOGSTR (INFO, "async_adaptor_0 threaded_cc") << "thread created " << pthread_self () << std::endl;

          t_cc->dump ();

          LOGSTR (INFO, "async_adaptor_0 threaded_cc") << "1 xxxxxxxxxxxxxxx" << std::endl;

          // we want to wrap a sync call...
          // FIXME: shouldn't we operate on a copy?  At least we need to make
          // sure that we reset the mode on failure, or otherwise log the fact
          // that the original mode was Async/Task...
          t_cc->set_mode (saga::async::Sync);

          LOGSTR (INFO, "async_adaptor_0 threaded_cc") << "2 xxxxxxxxxxxxxxx" << std::endl;

          int i = 0;
          while ( i < 10 )
          {
            LOGSTR (INFO, "async_adaptor_0 threaded_cc") << "thread is running " << i << std::endl;
            ::usleep (TASK_DELAY);
            i++;
          }

          LOGSTR (INFO, "async_adaptor_0 threaded_cc") << "3 xxxxxxxxxxxxxxx" << std::endl;

          saga::util::shared_ptr <saga::impl::engine> engine = t_cc->get_impl ()->get_engine ();

          LOGSTR (INFO, "async_adaptor_0 threaded_cc") << "4 xxxxxxxxxxxxxxx" << std::endl;

          engine->call /* <saga::impl::filesystem::file_cpi> */ (t_cc); // this will set task and call state
          // engine->call /* <cpi_t> */ (t_cc); // this will set task and call state

          LOGSTR (INFO, "async_adaptor_0 threaded_cc") << "6 xxxxxxxxxxxxxxx" << std::endl;

          // the sync call is now in Done state (or Failed etc), and its result is stored
          t_cc->dump ();

          LOGSTR (INFO, "async_adaptor_0 threaded_cc") << "thread done " << pthread_self () << std::endl;
        }
        catch ( const std::exception & e )
        {
          LOGSTR (ERROR, "threaded_cc catch") << "exception: " << e.what () << std::endl;
          t_cc->set_state (saga::async::Failed);
        }
        catch ( const char * msg )
        {
          LOGSTR (ERROR, "threaded_cc catch") << "exception: " << msg       << std::endl;
          t_cc->set_state (saga::async::Failed);
        }
      }


      async_adaptor_0::async_adaptor_0 (void) 
      { 
        SAGA_UTIL_STACKTRACE ();
      } 

      async_adaptor_0::~async_adaptor_0 (void) 
      {
        SAGA_UTIL_STACKTRACE ();
      } 

      void async_adaptor_0::constructor (saga::util::shared_ptr <saga::impl::call_context> cc)
      {
        // TODO: ensure impl->t_cc_->task_state is New

        SAGA_UTIL_STACKTRACE ();
        return;
      } 

      void async_adaptor_0::get_state (saga::util::shared_ptr <saga::impl::call_context> cc)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::async::state res_t;

        saga::util::shared_ptr <api_t>   impl  = cc->get_impl (); 

        // confirm result type
        cc->get_func ()->set_result <res_t> (impl->t_cc_->get_state ());

        cc->set_state (saga::async::Done);

        return;
      }

      void async_adaptor_0::get_result (saga::util::shared_ptr <saga::impl::call_context> cc)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::util::shared_ptr <saga::impl::result_base> res_t;

        saga::util::shared_ptr <api_t>   impl  = cc->get_impl (); 

        cc->get_func ()->set_result <res_t> (impl->t_cc_->get_func ()->get_result <res_t> ());
        cc->set_state          (saga::async::Done);

        return;
      }

      void async_adaptor_0::run (saga::util::shared_ptr <saga::impl::call_context> cc)
      {
        SAGA_UTIL_STACKTRACE ();

        saga::util::shared_ptr <api_t> impl  = cc->get_impl (); 

        // our new task exists, and impl->cc is the call it is operating
        // on.  That call's state depends on the runmode and the previous
        // state.
        //
        //   runmode   oldstate   newstate    notes
        //
        //   sync      new        done        need to run/wait
        //   sync      running    done        impossible
        //   sync      done       done
        //   sync      failed     failed
        //
        //   async     new        running     need to run
        //   async     running    running     
        //   async     done       done
        //   async     failed     failed
        //
        //   task      new        new
        //   task      running    running     
        //   task      done       done
        //   task      failed     failed
        //
        //   so, only 2 combinations require actions, and below we handle
        //   those

        // set call mode_ to Sync, because otherwise we will iterate
        // endlessly

        // FIXME: to_key can throw
        LOGSTR (INFO, "async_adaptor_0 ctor") 
          << "task_mode : " << saga::util::saga_enum_to_key <saga::async::mode>  (impl->t_cc_->get_mode ())  << std::endl
          << "task_state: " << saga::util::saga_enum_to_key <saga::async::state> (impl->t_cc_->get_state ()) << std::endl;

        // we don't do not spawn a thread for tasks which are not New -- that
        // effectively reconnects them though
        if ( impl->t_cc_->get_state () != saga::async::New )
        {
          return;
        }


        switch ( impl->t_cc_->get_mode () )
        {
          case saga::async::Sync:
            {
              // FIXME: this adaptor should actually not handle Sync calls...
              // impl->get_engine ()->call /* <cpi_t> */ (impl->t_cc_); 
              // impl->t_cc_->set_state (saga::async::Done);
              // break;
              throw "Cannot make sync calls async";
              break;
            }


          case saga::async::Async:
            {
              LOGSTR (INFO, "async_adaptor_0 ctor") << "== async task =====================================" << std::endl;

              pthread_t      thread;
              pthread_attr_t att;

              LOGSTR (INFO, "async_adaptor_0 ctor") << " async adaptor 0 : create new thread " << std::endl;

              // we lock the t_cc, and expect the thread
              impl->t_cc_->lock ();
              int err = pthread_create (&thread, NULL,
                                        saga::adaptor::test::async_adaptor_0::threaded_cc, 
                                        (void*)&(impl->t_cc_));

              if ( 0 != err )
              {
                LOGSTR (INFO, "async_adaptor_0 ctor") 
                  << " @@@ could not create thread: " << ::strerror (err) <<
                  std::endl;
                SAGA_UTIL_STACKDUMP ();
                /// impl->t_cc_->unlock (); // FIXME
                throw "oops";
              }

              // this second lock will wait for the thread to remove the first
              // lock, thus synchronizing.  Once that happened, we can
              // immediately unlock again...
              /// impl->t_cc_->lock   (); // FIXME
              /// impl->t_cc_->unlock (); // FIXME
              ::usleep (TASK_DELAY);

              // async call is done, thread state is set to Running, and then
              // maintained by the thread itself.
              // FIXME: sync with thread, so that any state setting there is done
              //        after the one below
              cc->set_state (saga::async::Done); 

              break;
            }

          case saga::async::Task:
          default:
            {
              LOGSTR (INFO, "async_adaptor_0 ctor") << " == no Task task yet =======================" << std::endl;
              break;
            }
        } // switch mode

      }

      void async_adaptor_0::wait (saga::util::shared_ptr <saga::impl::call_context> cc)
      {
        SAGA_UTIL_STACKTRACE ();

        saga::util::shared_ptr <api_t> impl  = cc->get_impl (); 

        while ( saga::async::Running == impl->t_cc_->get_state () )
        {
          ::usleep (TASK_DELAY);
        }

        cc->set_state (saga::async::Done); 
      }

    } // namespace test

  } // namespace adaptor

} // namespace saga

