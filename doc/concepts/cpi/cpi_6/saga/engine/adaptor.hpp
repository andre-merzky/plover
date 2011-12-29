
#ifndef SAGA_ENGINE_ADAPTOR_HPP
#define SAGA_ENGINE_ADAPTOR_HPP

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
  //////////////////////////////////////////////////////////////////////
  //
  // two independent CPI implementations, aka adaptors.
  //
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


      class file_adaptor_0 : public saga::impl::filesystem::file_cpi
      {
        private:
          typedef saga::impl::void_t                         void_t;
          typedef saga::impl::filesystem::file               api_t;
          typedef saga::impl::filesystem::file_cpi           cpi_t;
          typedef saga::impl::filesystem::file_instance_data idata_t;

        public:
          file_adaptor_0    (void) { } 
          ~file_adaptor_0   (void) { } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc, 
                              std::string                           url) 
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "file_adaptor_0 ctor") << "file adaptor 0 : constructor (" << url << ")" << std::endl;
            
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            cc->set_call_state (saga::impl::Done);

            return;
          } 

          void get_size (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            cc->set_call_state (saga::impl::Failed);

            LOGSTR (INFO, "file_adaptor_0 get_size") << "file adaptor 0 : get_size" << std::endl;
            throw "oops";
          }

          void get_size (saga::util::shared_ptr <saga::impl::call_context> cc, 
                         saga::impl::call_mode                             m)
          { 
            SAGA_UTIL_STACKTRACE ();

            cc->set_call_state (saga::impl::Failed);

            LOGSTR (INFO, "file_adaptor_0 get_size async") << "file adaptor 0 : get_size <async>" << std::endl;
            throw "oops";

          } 

          void copy (saga::util::shared_ptr <saga::impl::call_context> cc,
                     std::string                                       tgt)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            cc->set_call_state (saga::impl::Failed);

            LOGSTR (INFO, "file_adaptor_0 copy") << "file adaptor 0 : copy " << tgt << std::endl;
            throw "oops";
            return;
          }
      };


      class file_adaptor_1 : public saga::impl::filesystem::file_cpi
      {
        private:
          typedef saga::impl::void_t                         void_t;
          typedef saga::impl::filesystem::file               api_t;
          typedef saga::impl::filesystem::file_cpi           cpi_t;
          typedef saga::impl::filesystem::file_instance_data idata_t;

        public:
          file_adaptor_1    (void) { } 
          ~file_adaptor_1   (void) { } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                            std::string                                       url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "file_adaptor_1 ctor") << "file adaptor 1 : constructor (" << url << ")" << std::endl;
            
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            cc->set_call_state (saga::impl::Done);

            return;
          } 

          void get_size (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "file_adaptor_1 get_size") << "file adaptor 1 : get_size ()" << std::endl;
            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            struct stat buf;
            (void) stat (idata->url.c_str (), &buf);

            cc->set_result <int> (buf.st_size);
            cc->set_call_state (saga::impl::Done);
          }

          void get_size (saga::util::shared_ptr <saga::impl::call_context> cc, 
                         saga::impl::call_mode                             m)
          { 
            SAGA_UTIL_STACKTRACE ();
            LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== file adaptor 1 : get_size <> ()" << std::endl;

            cc->set_mode (m);

            cc->dump ();

            if ( m == saga::impl::Sync )
            {
              LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== file adaptor 1 : get_size <Sync> ()" << std::endl;

              // call the normal sync call, 
              // this is setting result and state
              get_size (cc); 
              cc->set_call_state (saga::impl::Done);

              saga::util::shared_ptr <saga::impl::task> ret (new saga::impl::task (cc));

              cc->set_result <saga::util::shared_ptr <saga::impl::task> > (ret);

              LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== get_size <Sync> () done ===== " << std::endl;
              return;
            }
            else if ( m == saga::impl::Async ||
                      m == saga::impl::Task  )
            {
              LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== file adaptor 1 : get_size <Async> ()" << std::endl;

              // async version: create a task straight away, and let the task 
              // adaptor deal with the async invocation of the sync call
              
              saga::util::shared_ptr <saga::impl::task> ret (new saga::impl::task (cc));

              cc->set_call_state (saga::impl::Done);

              cc->set_result <saga::util::shared_ptr <saga::impl::task> > (ret);

              LOGSTR (INFO, "file_adaptor_1 get_size") << " ===== get_size <Async> () done ===== " << std::endl;

              return;
            }

            throw "incorrect call mode!? - that should never happen!";
          } 

          void copy (saga::util::shared_ptr <saga::impl::call_context> cc,
                     std::string                                       tgt)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "file_adaptor_1 copy") << "file adaptor 1 : copy " << tgt << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            int res = ::system ((std::string ("cp ") + idata->url + " " + tgt).c_str ());

            if ( res != 0 )
            {
              cc->set_call_state (saga::impl::Failed);
              // cc->set_error ("system command error"); // TODO
            }
            else
            {
              cc->set_call_state (saga::impl::Done);
            }

            return;
          }
      };


      class dir_adaptor_0 : public saga::impl::filesystem::dir_cpi
      {
        private:
          typedef saga::impl::void_t                        void_t;
          typedef saga::impl::filesystem::dir               api_t;
          typedef saga::impl::filesystem::dir_cpi           cpi_t;
          typedef saga::impl::filesystem::dir_instance_data idata_t;

        public:
          dir_adaptor_0    (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 
          
          ~dir_adaptor_0   (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                            std::string                                       url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "dir_adaptor_0 ctor") << "dir adaptor 0 : constructor (" << url << ")" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            cc->set_call_state (saga::impl::Done);

            return;
          } 

          void get_url (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

            LOGSTR (INFO, "dir_adaptor_0 get_url") << "dir adaptor 0 : get_url" << std::endl;
            throw "oops";
          }
      };

      class dir_adaptor_1 : public saga::impl::filesystem::dir_cpi
      {
        private:
          typedef saga::impl::void_t                        void_t;
          typedef saga::impl::filesystem::dir               api_t;
          typedef saga::impl::filesystem::dir_cpi           cpi_t;
          typedef saga::impl::filesystem::dir_instance_data idata_t;

        public:
          dir_adaptor_1    (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 

          ~dir_adaptor_1   (void) 
          { 
            SAGA_UTIL_STACKTRACE ();
          } 

          void constructor (saga::util::shared_ptr <saga::impl::call_context> cc,
                            std::string                                       url) 
          { 
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <api_t> impl (cc->get_impl ()); 

            LOGSTR (INFO, "dir_adaptor_1 ctor") << "dir adaptor 1 : constructor (" << url << ")" << std::endl;

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();
            idata->url = url;

            cc->set_call_state (saga::impl::Done);

            return;
          } 

          void get_url (saga::util::shared_ptr <saga::impl::call_context> cc)
          {
            SAGA_UTIL_STACKTRACE ();
            saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            LOGSTR (INFO, "dir_adaptor_1 get_url") << "dir adaptor 1 : get_url: " << idata->url << std::endl;

            cc->set_result <std::string> (idata->url);
            cc->set_call_state (saga::impl::Done);

            return;
          }


          void open (saga::util::shared_ptr <saga::impl::call_context> cc, 
                     std::string                                       url)
          {
            SAGA_UTIL_STACKTRACE ();

            typedef saga::util::shared_ptr <saga::impl::filesystem::file> res_t;

            saga::util::shared_ptr <saga::impl::filesystem::dir> impl (cc->get_impl ());  

            saga::util::shared_ptr <idata_t> idata = impl->get_instance_data ();

            std::string new_url = idata->url + "/" + url;

            LOGSTR (INFO, "dir_adaptor_1 open") << "dir adaptor 1 : open: " << new_url << std::endl;

            // FIXME: error checks
            res_t ret (new saga::impl::filesystem::file);

            // FIXME: error checks
            // FIXME: should constructor be called by the impl::dir::open, or
            // here in the adaptor?  In the impl it can go to any adaptor, from
            // here as well...
            ret->constructor (new_url);

            cc->set_result <res_t> (ret);
            cc->set_call_state (saga::impl::Done);

            return;
          }
      };

    } // namespace test

  } // namespace adaptor


  //////////////////////////////////////////////////////////////////////
  //
  // API classes
  //
  class task
  {
    private:
      saga::util::shared_ptr <saga::impl::task> impl_;

    public:
      task (void)
        : impl_ (new saga::impl::task)
      {
        SAGA_UTIL_STACKTRACE ();
        (void) impl_->constructor ();
      }

      task (saga::util::shared_ptr <saga::impl::task> impl)
        : impl_ (impl)
      {
        SAGA_UTIL_STACKTRACE ();
        (void) impl_->constructor ();
      }

      saga::impl::call_state get_state (void)
      {
        SAGA_UTIL_STACKTRACE ();
        return impl_->get_state ();
      }

      template <typename T>
      T get_result (void)
      {
        SAGA_UTIL_STACKTRACE ();
        saga::util::shared_ptr <saga::impl::result_t> result = impl_->get_result (); 
        saga::util::shared_ptr <saga::impl::result_t_detail_ <T> > casted = result.get_shared_ptr <saga::impl::result_t_detail_ <T> > ();
        // FIXME: error check
        LOGSTR (DEBUG, "task get_result") << " 1 ##############################################################" << std::endl;
        LOGSTR (DEBUG, "task get_result") << " ------------------------------------ " << saga::util::demangle (typeid (T).name ()) << std::endl;
        impl_.dump ();
        LOGSTR (DEBUG, "task get_result") << " ------------------------------------ " << std::endl;
        impl_->dump ();
        LOGSTR (DEBUG, "task get_result") << " 2 ##############################################################" << std::endl;

        T t = casted->get ();
        LOGSTR (DEBUG, "task get_result") << " 3 ##############################################################" << std::endl;
        return t;
      }
  };


  namespace filesystem
  {
    class file
    {
      private:
        saga::util::shared_ptr <saga::impl::filesystem::file> impl_;

      public:
        file (std::string url)
          : impl_ (new saga::impl::filesystem::file)
        {
          SAGA_UTIL_STACKTRACE ();
          (void) impl_->constructor (url);
        }

        file (saga::util::shared_ptr <saga::impl::filesystem::file> impl)
          : impl_ (impl)
        {
          SAGA_UTIL_STACKTRACE ();
          // FIXME: where to get the URL/idata?  Is constructor here really useful,
          // as impl already exists?  Is impl always initialized?
          // In general, need to clear up who is filling idata, when the object
          // is adaptor initialized / bound (void) 
          //
          // impl_->constructor (url);
        }

        int get_size (void)
        {
          SAGA_UTIL_STACKTRACE ();
          return impl_->get_size ();
        }

        template <enum saga::impl::call_mode M>
        saga::task get_size (void)
        {
          SAGA_UTIL_STACKTRACE ();
          return saga::task (impl_->get_size (M));
        }

        void copy (std::string tgt)
        {
          SAGA_UTIL_STACKTRACE ();
          (void) impl_->copy (tgt);
        }
    };

    class dir
    {
      private:
        saga::util::shared_ptr <saga::impl::filesystem::dir> impl_;

      public:
        dir (std::string url)
          : impl_ (new saga::impl::filesystem::dir)
        {
          SAGA_UTIL_STACKTRACE ();
          (void) impl_->constructor (url);
        }

        std::string get_url (void)
        {
          SAGA_UTIL_STACKTRACE ();
          return impl_->get_url ();
        }

        saga::filesystem::file open (std::string url)
        {
          SAGA_UTIL_STACKTRACE ();
          return saga::filesystem::file (impl_->open (url));
        }
    };

  } // namespace filesystem

} // namespace saga

#endif //  SAGA_ENGINE_ADAPTOR_HPP

