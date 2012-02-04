
#include <saga/util/enums.hpp>

#include <saga/adaptor/test/filesystem_adaptor_0.hpp>
#include <saga/adaptor/test/filesystem_adaptor_1.hpp>
#include <saga/adaptor/test/async_adaptor_0.hpp>

#include <saga/engine/engine.hpp>

namespace saga
{
  namespace impl
  {
    // default engine is actually a singleton
    saga::util::shared_ptr <engine> & engine::the_engine (void)
    {
      static saga::util::shared_ptr <engine> the_engine_ (new engine ());
      return the_engine_;
    }

    // c'tor, loads adaptors
    engine::engine (void)
    {
      SAGA_UTIL_STACKTRACE ();

      LOGSTR (INFO, "engine") << "engine: register adaptors";

      // create and register adaptor instances
      cpis_.push_back (saga::util::shared_ptr <saga::impl::cpi_base> (new saga::adaptor::test::file_adaptor_0 ));
      cpis_.push_back (saga::util::shared_ptr <saga::impl::cpi_base> (new saga::adaptor::test::file_adaptor_1 ));
      cpis_.push_back (saga::util::shared_ptr <saga::impl::cpi_base> (new saga::adaptor::test::dir_adaptor_0  ));
      cpis_.push_back (saga::util::shared_ptr <saga::impl::cpi_base> (new saga::adaptor::test::dir_adaptor_1  ));
      cpis_.push_back (saga::util::shared_ptr <saga::impl::cpi_base> (new saga::adaptor::test::async_adaptor_0));
    }


    void engine::call (saga::util::shared_ptr <saga::impl::call_context> cc)
    {
      SAGA_UTIL_STACKTRACE ();

      {
        LOGSTR (INFO, "engine call") << "calling cpis " << cpis_.size () << std::endl;
        // cc->dump ();

        // try one adaptor after the other, until one succeeds.
        for ( unsigned int i = 0; i < cpis_.size (); i++ )
        {
          LOGSTR (INFO, "engine call") << "calling cpi " << i << " / " << cpis_.size () << std::endl;
          try
          {
            LOGSTR (INFO, "engine call") << "adaptor " << i << " : calling " << cc->get_func ()->get_name () << std::endl;
            cpis_[i]->dump ();

            cc->get_func ()->call_cpi (cpis_[i], cc);
            cc->set_state (saga::async::Done);
            LOGSTR (INFO, "engine call") << "adaptor " << i << " : succeeded for " << cc->get_func ()->get_name () << std::endl;

            return;
          }
          catch ( const char * m )
          {
            LOGSTR (INFO, "engine call") << "adaptor " << i << " : failed for " << cc->get_func ()->get_name () << " : " << m << std::endl;
          }
          catch ( ... )
          {
            LOGSTR (INFO, "engine call") << "adaptor " << i << " : failed for " << cc->get_func ()->get_name () << " : ???" << std::endl;
          }
          LOGSTR (INFO, "engine call") << "calling cpi done " << i << " / " << cpis_.size () << std::endl;
        }
      }

      {
        // if a Async or Task cc failed, lets see if we find a task adaptor
        // which can run it.  That is only done if the CC is not already
        // representing a saga::async::task::run() method

        typedef saga::impl::void_t                       res_t;
        typedef saga::impl::async::task                  api_t;
        typedef saga::impl::async::task_cpi              cpi_t;
        typedef saga::impl::func_0 <api_t, cpi_t, res_t> func_t;

        if ( cc->get_func ()->has_cpi_type <cpi_t> () /* &&
             cc->get_func ()->get_name () == "run"    */ )
        {
          // go along, nothing to see here...
        }
        else
        {
          // create a call context to run task.run
          saga::util::shared_ptr <func_t> t_func (new func_t ("run", &cpi_t::run));

          // create a new task object which operates on the cc
          saga::util::shared_ptr <api_t> task (new saga::impl::async::task (cc, cc->get_impl ()->get_engine ()));

          saga::util::shared_ptr <saga::impl::call_context> t_cc (new saga::impl::call_context (task, t_func)); 

          t_cc->set_policy (saga::impl::call_context::Any);
          t_cc->set_mode   (saga::async::Sync);
          t_cc->set_state  (saga::async::New);

          // we now call ourself once more, with the explicit task based cc.  

          LOGSTR (INFO, "engine call") << "=======================================" << std::endl;
          t_cc->dump ();
          LOGSTR (INFO, "engine call") << "=======================================" << std::endl;

          call (t_cc);

          // done 
          return;
        }
      }


      // no (direct nor async::task) adaptor suceeded.  We don't have anything
      // sensible to return, so we flag the failure, and throw.  That is
      // redundant, but hey...  (FIXME)
      cc->set_state (saga::async::Failed);

      LOGSTR (INFO, "engine call") << "all adaptors failed for " << cc->get_func ()->get_name () << std::endl;
      SAGA_UTIL_STACKDUMP ();
      throw "no adaptor suceeded";
    }


    void engine::dump (std::string msg)
    {
      LOGSTR (DEBUG, "engine dump") 
        << "engine (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) << " : " << msg << std::endl;

      for ( unsigned int i = 0; i < cpis_.size (); i++ )
      {
        cpis_[i].dump();
      }
    }

  } // namespace impl

} // namespace saga

