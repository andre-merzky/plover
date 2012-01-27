
#ifndef SAGA_ENGINE_CPIBASE_HPP
#define SAGA_ENGINE_CPIBASE_HPP

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

namespace saga
{
  namespace impl
  {
    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all CPIs
    //
    class cpi_base : public saga::util::shareable
    {
      private:
        std::map <std::string, void*> adata_;  // FIXME: make extensible and use

      public:
        virtual ~cpi_base (void);

        virtual void dump (std::string msg = "");
    };

  } // namespace impl

} // namespace saga

#endif //  SAGA_ENGINE_CPIBASE_HPP

