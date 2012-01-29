
#ifndef SAGA_ENGINE_HPP
#define SAGA_ENGINE_HPP

#include <string>
#include <map>
#include <vector>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/engine/func.hpp>
#include <saga/engine/call_context.hpp>
#include <saga/engine/cpi_base.hpp>
#include <saga/engine/impl_base.hpp>

#include <saga/cpi/async/task.hpp>
#include <saga/impl/async/task.hpp>

namespace saga
{
  namespace impl
  {
    // forward decl
    class call_context;

    //////////////////////////////////////////////////////////////////////
    //
    // the SAGA Engine implementation, which loads all adaptors, and 
    // supports the SAGA api/cpi implementations.
    //
    // In reality, adaptor registration will be more complex:  if an adaptor
    // registers, the engine will inspect it, to register it for all CPI's it
    // knows about:
    //
    //   if ( adaptor.is_a <saga_object> () )
    //     object_cpis.push_back (adaptor);
    //   if ( adaptor.is_a <saga_attribute> () )
    //     attribute_cpis.push_back (adaptor);
    //   ...
    //
    // or something slightly more clever ;-)
    //
    class engine : public saga::util::shareable
    {
      private:
        std::vector <saga::util::shared_ptr <saga::impl::cpi_base> > cpis_;

      public:
        engine (void);

        template <typename ADP>
        saga::util::shared_ptr <saga::impl::cpi_base> open_adaptor (void)
        {
          SAGA_UTIL_STACKTRACE ();
          return saga::util::shared_ptr <saga::impl::cpi_base> (new ADP);
        }


        // get_cpi's is used by the API class implementation to get an (ordered)
        // list of adaptors to call
        template <typename CPI>
        std::vector <saga::util::shared_ptr <CPI> > get_cpis (void)
        {
          std::vector <saga::util::shared_ptr <CPI> > ret;

          for ( unsigned int i = 0; i < cpis_.size (); i++ )
          {
            if ( cpis_[i].is_a <CPI> () )
            {
              ret.push_back (cpis_[i].get_shared_ptr <CPI> ());
            }
          }
          
          return ret;
        }


        //////////////////////////////////////////////////////////////////
        //
        // 
        //
        /* template <typename CPI> */
        void call (saga::util::shared_ptr <saga::impl::call_context> cc)
        {
          SAGA_UTIL_STACKTRACE ();

          {
            // get the matching list of CPIs
         /* std::vector <saga::util::shared_ptr <CPI> > cpis = get_cpis <CPI> (); */
            std::vector <saga::util::shared_ptr <saga::impl::cpi_base> > cpis = cpis_;

            // try one adaptor after the other, until one succeeds.
            LOGSTR (INFO, "engine call") << "calling cpis " << cpis.size () << std::endl;
         /* LOGSTR (INFO, "engine call") << "calling cpis " << saga::util::demangle (typeid (CPI).name ()) << std::endl; */
            // cc->dump ();
            
            for ( unsigned int i = 0; i < cpis.size (); i++ )
            {
              LOGSTR (INFO, "engine call") << "calling cpi " << i << " / " << cpis.size () << std::endl;
              try
              {
                LOGSTR (INFO, "engine call") << "adaptor " << i << " : calling " << cc->get_func ()->get_name () << std::endl;
                cpis[i]->dump ();

                cc->get_func ()->call_cpi (cpis[i], cc);
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
              LOGSTR (INFO, "engine call") << "calling cpi done " << i << " / " << cpis.size () << std::endl;
            }
          }

          {
            // if a Async or Task cc failed, lets see if we find a task adaptor
            // which can run it.

            // create a call context to run task.run
            typedef saga::impl::void_t                          res_t;
            typedef saga::impl::async::task                     api_t;
            typedef saga::impl::async::task_cpi                 cpi_t;
            typedef saga::impl::func_0 <api_t, cpi_t, res_t> func_t;

            std::vector <saga::util::shared_ptr <cpi_t> > cpis = get_cpis <cpi_t> ();

            saga::util::shared_ptr <func_t> t_func (new func_t ("run", &cpi_t::run));

            // create a new task object which operates on the cc
            saga::util::shared_ptr <api_t> task (new saga::impl::async::task (cc, cc->get_impl ()->get_engine ()));

            saga::util::shared_ptr <saga::impl::call_context> t_cc (new saga::impl::call_context (task, t_func)); 

            LOGSTR (INFO, "engine call") << "=======================================" << std::endl;
            t_cc->dump ();
            LOGSTR (INFO, "engine call") << "=======================================" << std::endl;

            // try one adaptor after the other, until one succeeds.
            LOGSTR (INFO, "engine call") << "calling cpis " << cpis.size () << std::endl;
         /* LOGSTR (INFO, "engine call") << "calling cpis " << saga::util::demangle (typeid (CPI).name ()) << std::endl; */
            // cc->dump ();
            
            for ( unsigned int i = 0; i < cpis.size (); i++ )
            {
              LOGSTR (INFO, "engine call") << "calling cpi " << i << " / " << cpis.size () << std::endl;
              try
              {
                LOGSTR (INFO, "engine call") << "adaptor " << i << " : calling " << t_func->get_name () << std::endl;
                cpis[i]->dump ();

                // the cast below ensures that the t_func has the correct type, kind of
                // typedef saga::impl::func <IMPL, CPI, RET> func_cast_t;
                // saga::util::shared_ptr <func_cast_t> casted = t_func.get_shared_ptr <func_cast_t> ();
                // casted->call_cpi (cpis[i], cc);

                t_func->call_cpi (cpis[i], t_cc);
                t_cc->set_state (saga::async::Done);
                LOGSTR (INFO, "engine call") << "adaptor " << i << " : succeeded for " << t_func->get_name () << std::endl;

                return;
              }
              catch ( const char * m )
              {
                LOGSTR (INFO, "engine call") << "adaptor " << i << " : failed for " << t_func->get_name () << " : " << m << std::endl;
              }
              catch ( ... )
              {
                LOGSTR (INFO, "engine call") << "adaptor " << i << " : failed for " << t_func->get_name () << " : ???" << std::endl;
              }
              LOGSTR (INFO, "engine call") << "calling cpi done " << i << " / " << cpis.size () << std::endl;
            }
          }


          // no (sync nor async) adaptor suceeded.  We don't have anything
          // sensible to return, so we flag the failure, and throw.  That is
          // redundant, but hey...  (FIXME)
          cc->set_state (saga::async::Failed);

          LOGSTR (INFO, "engine call") << "all adaptors failed for " << cc->get_func ()->get_name () << std::endl;
          SAGA_UTIL_STACKDUMP ();
          throw "no adaptor suceeded";
        }

        void dump (std::string msg = "");

        static saga::util::shared_ptr <engine> & the_engine (void); 
    };

  } // namespace impl

} // namespace saga

#endif //  SAGA_ENGINE_HPP

