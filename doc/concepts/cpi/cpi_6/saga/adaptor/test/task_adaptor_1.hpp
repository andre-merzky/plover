
#ifndef SAGA_ADAPTOR_TASK_0_HPP
#define SAGA_ADAPTOR_TASK_0_HPP

#include <map>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <saga/util/mutex.hpp>
#include <saga/util/scoped_lock.hpp>
#include <saga/util/shareable.hpp>
#include <saga/util/enums.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

namespace saga
{
  namespace adaptor
  {
    namespace test // test adaptprs
    {
      class task_adaptor_0 : public saga::impl::task_cpi
      {
        private:
          typedef saga::impl::void_t             void_t;
          typedef saga::impl::task               api_t;
          typedef saga::impl::task_cpi           cpi_t;
          typedef saga::impl::task_instance_data idata_t;

          static void * threaded_cc (void * cc_sp)
          {
            saga::util::shared_ptr <saga::impl::call_context> cc; 
            try
            {
              LOGSTR (INFO, "task_adaptor_0 threaded_cc") << " @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;

              SAGA_UTIL_STACKTRACE ();
              saga::util::shared_ptr <saga::impl::call_context> * cc_tmp
                = static_cast <saga::util::shared_ptr <saga::impl::call_context> *> (cc_sp);

              typedef saga::impl::task     api_t;
              typedef saga::impl::task_cpi cpi_t;
              typedef saga::impl::task_cpi ret_t;

              cc = cc_tmp->get_shared_ptr (); 

              LOGSTR (INFO, "task_adaptor_0 threaded_cc") << "thread created " << pthread_self () << std::endl;

              cc->dump ();

              // wait 'til task is getting run()
              while ( cc->get_task_state () == saga::impl::New )
              {
                ::sleep (3); // FIXME: nanosleep, configurable timeout
              }

              if ( cc->get_task_state () == saga::impl::Running )
              {
                LOGSTR (INFO, "task_adaptor_0 threaded_cc") << "thread starting " << pthread_self () << std::endl;

                saga::util::shared_ptr <saga::impl::impl_base> impl (cc->get_impl ()); 

                LOGSTR (INFO, "task_adaptor_0 threaded_cc") << " 1 xxxxxxxxxxxxxxx" << std::endl;

                impl->dump ("impl sp: ");

                LOGSTR (INFO, "task_adaptor_0 threaded_cc") << " 2 xxxxxxxxxxxxxxx" << std::endl;

                saga::util::shared_ptr <saga::impl::engine> engine = impl->get_engine ();

                // engine.dump  ();
                // engine->dump ();

                LOGSTR (INFO, "task_adaptor_0 threaded_cc") << " 3 xxxxxxxxxxxxxxx" << std::endl;

                int i = 0;
                while ( i < 10 )
                {
                  LOGSTR (INFO, "task_adaptor_0 threaded_cc") << "thread is running " << i << std::endl;
                  ::sleep (1);
                  i++;
                }

                LOGSTR (INFO, "task_adaptor_0 threaded_cc") << " 4 xxxxxxxxxxxxxxx" << std::endl;

                // FIXME: now, we need to actually split
                // the call_context into call_context, op_context, and
                // task_context, like:
                //
                //   task t = get_size <async> ();
                //
                //  the (async get_size) call is Done when a valid task object returned
                //  (call_context).
                //
                //  The op is done when the task has the resulting size stored
                //  (op_context) --> maps to task state / context.  
                //
                //  An op thus represents some adaptor operation.  But, a task
                //  state might be comprised of multiple op states (same for
                //  sync calls btw) - thus the split again...
                //
                //  Anyway, for this quick demo, we simply reset the cc to
                //  Sync/New
                //
             // cc->get_func ()->set_arg_1 (saga::impl::Sync);
                cc->set_mode               (saga::impl::Sync);
                cc->set_call_state         (saga::impl::New);
                cc->dump ();

                LOGSTR (INFO, "task_adaptor_0 threaded_cc") << " 5 xxxxxxxxxxxxxxx" << std::endl;

                engine->call <api_t, cpi_t> (cc); // this will set task and call state

                LOGSTR (INFO, "task_adaptor_0 threaded_cc") << " 6 xxxxxxxxxxxxxxx" << std::endl;

                // the sync call is now Done, and its result is stored
              }

              LOGSTR (INFO, "task_adaptor_0 threaded_cc") << "thread done " << pthread_self () << std::endl;
            }
            catch ( const std::exception & e )
            {
              LOGSTR (ERROR, "threaded_cc catch") << "exception: " << e.what () << std::endl;
              cc->set_task_state (saga::impl::Failed);
            }
            catch ( const char * msg )
            {
              LOGSTR (ERROR, "threaded_cc catch") << "exception: " << msg       << std::endl;
              cc->set_task_state (saga::impl::Failed);
            }
          }


        public:
          task_adaptor_0 (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 

          ~task_adaptor_0 (void) 
          {
            SAGA_UTIL_STACKTRACE ();
          } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            // TODO: ensure idata->t_cc->task_state is New or Unknown

            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "task_adaptor_0 ctor") 
              << "task adaptor 0 : constructor ()" << std::endl;
            
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            LOGSTR (INFO, "task_adaptor_0 ctor") 
              << " ==idata cc =====================================================" << std::endl;
            idata->t_cc->dump ();
            LOGSTR (INFO, "task_adaptor_0 ctor") 
              << " ================================================================" << std::endl;

            // our new task exists, and idata->cc is the call it is operating
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

            // set call mode to Sync, because otherwise we will iterate
            // endlessly

            // FIXME: to_key can throw
            LOGSTR (INFO, "task_adaptor_0 ctor") 
              << "cpi_mode  : " << saga::util::saga_enum_to_key <saga::impl::call_mode>  (idata->t_cc->get_mode ())       << std::endl
              << "task_state: " << saga::util::saga_enum_to_key <saga::impl::call_state> (idata->t_cc->get_task_state ()) << std::endl;


            if ( idata->t_cc->get_mode       () == saga::impl::Sync &&
                 idata->t_cc->get_task_state () == saga::impl::New  )
            {
              LOGSTR (INFO, "task_adaptor_0 ctor") 
                << " == sync task =====================================================" << std::endl;

              // idata->t_cc->task_state is Done, cc->call_state is set by call()
              impl->get_engine ()->call <api_t, cpi_t> (idata->t_cc); 
              idata->t_cc->set_task_state (saga::impl::Done);
            }
            else if ( idata->t_cc->get_mode       () == saga::impl::Async &&
                      idata->t_cc->get_task_state () == saga::impl::New   )
            {
              LOGSTR (INFO, "task_adaptor_0 ctor") 
                << " == async task =====================================================" << std::endl;
              pthread_t      thread;
              pthread_attr_t att;

              LOGSTR (INFO, "task_adaptor_0 ctor") 
                << " task adaptor 0 : create new thread " << std::endl;

              saga::util::shared_ptr <saga::impl::call_context> * tmp = new saga::util::shared_ptr <saga::impl::call_context> (idata->t_cc);

              int err = pthread_create (&thread, NULL,
                                        saga::adaptor::test::task_adaptor_0::threaded_cc, 
                                        (void*)&(idata->t_cc));
              if ( 0 != err )
              {
                LOGSTR (INFO, "task_adaptor_0 ctor") 
                  << " @@@ could not create thread: " << ::strerror (err) <<
                  std::endl;
                throw "oops";
              }

              // async call is done, thread state is set to Running, and then
              // maintained by the thread itself.
              // FIXME: sync with thread, so that any state setting there is done
              //        after the one below
              cc->set_call_state (saga::impl::Done); 
              idata->t_cc->set_task_state (saga::impl::Running);
            }
            else
            {
              LOGSTR (INFO, "task_adaptor_0 ctor") 
                << " == no   task =====================================================" << std::endl;
            }

            return;
          } 

          void get_state (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();

            typedef saga::impl::call_state res_t;

            saga::util::shared_ptr <saga::impl::task> impl (cc->get_impl ()); 

            LOGSTR (INFO, "task_adaptor_0 get_state") 
              << "task adaptor 0 : get_state ()" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            cc->set_result <res_t> (idata->t_cc->get_task_state ());
            cc->set_call_state (saga::impl::Done);

            return;
          }

          void get_result (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();

            typedef saga::util::shared_ptr <saga::impl::result_t> res_t;

            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "task_adaptor_0 get_result") 
              << "task adaptor 0 : get_result ()" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            LOGSTR (INFO, "task_adaptor_0 get_result") 
              << "=-----------------------------------------------" << std::endl;
            idata->t_cc->dump ();
            LOGSTR (INFO, "task_adaptor_0 get_result") 
              << "=-----------------------------------------------" << std::endl;

            cc->set_result <res_t> (idata->t_cc->get_result ());
            cc->set_call_state (saga::impl::Done);

            return;
          }
      };

    } // namespace test

  } // namespace adaptor

} // namespace saga

#endif //  SAGA_ADAPTOR_TASK_0_HPP

