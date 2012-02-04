
#ifndef SAGA_ENGINE_HPP
#define SAGA_ENGINE_HPP

#include <string>
#include <map>
#include <vector>

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

#include <saga/engine/call_context.hpp>
#include <saga/engine/cpi_base.hpp>

namespace saga
{
  namespace impl
  {
    //////////////////////////////////////////////////////////////////////
    //
    // The SAGA Engine implementation, which loads all adaptors, and 
    // supports the SAGA api/cpi implementations.
    //
    // The engine provides two functionalities:
    //
    //   - (i)  loading / managing adaptors
    //   - (ii) translating API calls into Adaptor calls
    //
    // Adaptor loading is done on construction - but note that the engine is not
    // necessarily a singleton, so instances can be created at will, and
    // different adaptor sets can be loaded and maintained if needed (the
    // default use case will likely be to have a single instance though).
    //
    // API call translation is done in engine.call(call_context cc).  That
    // method implements several call policies.  Those are, at the moment:
    //
    //  - Any    : the call is considered done when any adaptor suceeded --
    //             adaptors are tried one after the other
    //  - Bound  : the call MUST be performed by the last succeeding adaptor --
    //             even if it fails, no other adaptor can be used (not yet
    //             implemented).
    //  - Collect: All adaptors are used, and the individual results are
    //             combined -- that requires the result type to be combinable
    //             (i.e. a list).  Adaptor invocations may be done
    //             asynchronously (not yet implemented).
    //
    // An API cc can be synchronous or asynchronous.  The adaptor level
    // methods on the other hand are always considered asynchronous -- but MAY
    // return completed (thus rendering them effectively synchronous).
    //
    // Any cc will pass three rounds of adaptor selections / invokations:
    //
    //  - (ii) Filtering:
    //
    //    invokes adaptors can modify the cc, but will never be able to actually
    //
    // If call() is enacting a asynchronous call, but no adaptor succeeds, then
    // it will invoke a special async::task cpi, which MAY be able to wrap
    // a synchronous cc into a thread, for application level asynchronous
    // execution.
    //
    // The current engine has a number of shortcomings, like:
    // 
    //  - adaptor registration is static. In reality, adaptor registration will 
    //    be more complex:  if an adaptor registers, the engine will inspect it, 
    //    to register it for all CPI's it knows about:
    //
    //    if ( adaptor.is_a <saga_object> () )
    //      object_cpis.push_back (adaptor);
    //    if ( adaptor.is_a <saga_attribute> () )
    //      attribute_cpis.push_back (adaptor);
    //    ...
    //
    //    or something slightly more clever ;-)
    //
    //
    //  - no adaptor ordering is implemented
    //
    //    successfull adaptors should be reused on the same impl instance.
    //    Also, filter adaptors need to be run before other adaptors, and need
    //    to be able to signal success/failure to call().
    //
    class engine : public saga::util::shareable
    {
      private:
        std::vector <saga::util::shared_ptr <saga::impl::cpi_base> > cpis_;

      public:
        engine (void);

        void call (saga::util::shared_ptr <saga::impl::call_context> cc);

        void dump (std::string msg = "");

        // singleton default engine instance
        static saga::util::shared_ptr <engine> & the_engine (void); 
    };

  } // namespace impl

} // namespace saga

#endif //  SAGA_ENGINE_HPP

