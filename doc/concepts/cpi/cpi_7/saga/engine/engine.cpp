
#include <saga/util/enums.hpp>

#include <saga/adaptor/test/filesystem_adaptor_0.hpp>
#include <saga/adaptor/test/filesystem_adaptor_1.hpp>
#include <saga/adaptor/test/async_adaptor_0.hpp>

#include "engine.hpp"

static saga::util::shared_ptr <saga::impl::engine> the_engine (new saga::impl::engine);

namespace saga
{
  namespace impl
  {
    SAGA_UTIL_REGISTER_ENUM (cpi_mode  , Any       , 0);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode  , Bounde    , 1);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode  , Simple    , 2);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode  , Collective, 3);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode  , Filter    , 4);
    SAGA_UTIL_REGISTER_ENUM (cpi_mode  , All       , 5);


    std::ostream & operator<< (std::ostream & o, void_t const & v)
    {
      return o << "[void_t]";
    }


    //////////////////////////////////////////////////////////////////
    //
    // the result type thingies allow to keep get_result template free on impl
    // level and below - the template free base class is used wherever possible, 
    // and the types result_t_detail_ is only aquired as needed.
    //
    result_t::~result_t (void) 
    {
      SAGA_UTIL_STACKTRACE ();
    }

    void result_t::dump (std::string msg) 
    { 
      LOGSTR (DEBUG, "dump, result_t") << msg << " - (base result type)" << std::endl;
    }



    //////////////////////////////////////////////////////////////////
    functor_base::functor_base (std::string name)
      : name_ (name)
    {
      SAGA_UTIL_STACKTRACE ();
    }

    functor_base::~functor_base (void) 
    {
      SAGA_UTIL_STACKTRACE ();
    }

    saga::util::shared_ptr <saga::impl::result_t> functor_base::get_result  (void) 
    {
      return result_;
    }

    std::string functor_base::get_name (void) 
    {
      return name_;  
    }

    void functor_base::dump (std::string msg)
    {
      LOGSTR (DEBUG, "functor_base dump")
        << " (" << this << ") : " 
        << saga::util::demangle (typeid (*this).name ()) 
        << std::endl;
    }


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
        , result_      ((new saga::impl::result_t_detail_ <saga::impl::void_t> ()))
    {
      SAGA_UTIL_STACKTRACE ();
    }

    saga::util::shared_ptr <functor_base>           call_context::get_func (void)  { return func_;      } 
    saga::util::shared_ptr <saga::util::shareable>  call_context::get_impl (void)  { return impl_;      } 

    void                   call_context::set_state (call_context::state s)         { call_state_ = s;   }
    call_context::state    call_context::get_state (void)                          { return call_state_;}

    saga::util::shared_ptr <saga::impl::result_t> call_context::get_result (void)
    {
      return result_;
    }

    void call_context::dump (std::string msg)
    {
      // FIXME: to_key can throw
      LOGSTR (DEBUG, "call_context dump")
        <<   "call_context (" << this << ") : " << msg << std::endl
        <<   "    cpi  mode   : " << saga::util::saga_enum_to_key <saga::impl::cpi_mode>            (cpi_mode_  ) << std::endl
        <<   "    call state  : " << saga::util::saga_enum_to_key <saga::impl::call_context::state> (call_state_) << std::endl
        <<   "    func name   : " << func_name_ << std::endl
        <<   "    func args   : " << func_args_ << std::endl;
      impl_.dump         ("    IMPL_       : ");
      impl_->dump        ();
      result_.dump       ("    result_: ");
      result_->dump      ();
      func_.dump         ("    func_       : ");
      func_->dump        ();
    }


    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all IMPLs
    //
    impl_base::impl_base (void)
      : engine_ (the_engine) // FIXME: no need to create a new engine - get it from the session!
    {
    }

    impl_base::~impl_base (void) 
    {
    }

    void impl_base::dump (std::string msg)
    {
      LOGSTR (DEBUG, "impl_base dump") << "impl_base (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) 
                << " : " << msg  << std::endl;
      engine_.dump ("    engine_     : ");
      engine_->dump();
    }

    saga::util::shared_ptr <saga::impl::engine> impl_base::get_engine (void) 
    { 
      return engine_; 
    }

    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all CPIs
    //
    cpi_base::~cpi_base (void) {}

    void cpi_base::dump (std::string msg)
    {
      LOGSTR (DEBUG, "cpi_base dump")
        << "cpi_base (" << this << ") : " 
            << saga::util::demangle (typeid (*this).name ()) << " : " 
               << msg << std::endl;
    }



    //////////////////////////////////////////////////////////////////////
    //
    // the SAGA Engine implementation, which loads all adaptors, and 
    // supports the SAGA file implementation.
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
    engine::engine (void)
    {
      SAGA_UTIL_STACKTRACE ();

      LOGSTR (INFO, "engine") << "engine: register all adaptors";

      // create and register adaptor instances
      cpis_.push_back (open_adaptor <saga::adaptor::test::file_adaptor_0>  ());
      cpis_.push_back (open_adaptor <saga::adaptor::test::file_adaptor_1>  ());
      cpis_.push_back (open_adaptor <saga::adaptor::test::dir_adaptor_0>   ());
      cpis_.push_back (open_adaptor <saga::adaptor::test::dir_adaptor_1>   ());
      cpis_.push_back (open_adaptor <saga::adaptor::test::async_adaptor_0> ());
    }


    // void engine::call (saga::util::shared_ptr <saga::impl::call_context> cc)
    // {
    //   SAGA_UTIL_STACKTRACE ();
    //
    //   typedef saga::impl::functor_base             func_base_t;
    //
    //   // try one adaptor after the other, until one succeeds.
    //   LOGSTR (INFO, "engine call") << "calling cpis " << cpis_.size () << std::endl;
    //   cc->dump ();
    //
    //   for ( unsigned int i = 0; i < cpis_.size (); i++ )
    //   {
    //     LOGSTR (INFO, "engine call") << "calling cpi " << i << " / " << cpis_.size () << " - " << typeid (cpis_[i]).name () << std::endl;
    //     try
    //     {
    //       LOGSTR (INFO, "engine call") << "adaptor " << i << " : calling " << cc->get_func()->get_name () << std::endl;
    //       cpis_[i]->dump ();
    //
    //       saga::util::shared_ptr <func_base_t> func   = cc->get_func ();
    //       // saga::util::shared_ptr <func_cast_t> casted = func.get_shared_ptr <func_cast_t> ();
    //
    //       func->call_cpi (cpis_[i], cc);
    //       cc->set_state (saga::impl::call_context::Done);
    //       LOGSTR (INFO, "engine call") << "adaptor " << i << " : succeeded for " << cc->get_func()->get_name () << std::endl;
    //
    //       return;
    //     }
    //     catch ( const char * m )
    //     {
    //       LOGSTR (INFO, "engine call") << "adaptor " << i << " : failed for " << cc->get_func()->get_name () << " : " << m << std::endl;
    //     }
    //     catch ( ... )
    //     {
    //       LOGSTR (INFO, "engine call") << "adaptor " << i << " : failed for " << cc->get_func()->get_name () << " : ???" << std::endl;
    //     }
    //     LOGSTR (INFO, "engine call") << "calling cpi done " << i << " / " << cpis_.size () << std::endl;
    //   }

    //   // no adaptor suceeded.  We don't have anything sensible to return, so
    //   // we flag the failure, and throw.  That is redundant, but hey...
    //   cc->set_state (saga::impl::call_context::Failed);

    //   LOGSTR (INFO, "engine call") << "all adaptors failed for " << cc->get_func()->get_name () << std::endl;
    //   throw "no adaptor suceeded";
    // }


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

