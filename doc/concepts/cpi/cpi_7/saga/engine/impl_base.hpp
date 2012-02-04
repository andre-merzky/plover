
#ifndef SAGA_ENGINE_IMPLBASE_HPP
#define SAGA_ENGINE_IMPLBASE_HPP

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

namespace saga
{
  namespace impl
  {
    class engine;

    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all IMPLs
    //
    class impl_base : public saga::util::shareable
    {
      protected:
        saga::util::shared_ptr <saga::impl::engine> engine_;

      public:
        impl_base (void);

        virtual ~impl_base (void);

        saga::util::shared_ptr <saga::impl::engine> get_engine (void);

        void dump (std::string msg = "");
    };

  } // namespace impl

} // namespace saga

#endif //  SAGA_ENGINE_IMPLBASE_HPP

