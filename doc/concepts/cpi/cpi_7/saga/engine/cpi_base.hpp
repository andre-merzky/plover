
#ifndef SAGA_ENGINE_CPIBASE_HPP
#define SAGA_ENGINE_CPIBASE_HPP

#include <saga/util/shareable.hpp>
#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

namespace saga
{
  namespace impl
  {
    // mode per cpi call
    enum cpi_mode 
    {
      Any        = 0, 
      Bound      = 1, 
      Simple     = 2, 
      Collective = 3, 
      Filter     = 4,
      All        = 5
    };

    //////////////////////////////////////////////////////////////////
    //
    // father and grandfather of all CPIs
    //
    class cpi_base : public saga::util::shareable
    {
      private:
        std::map <std::string, void*> adata_;  // FIXME: check and use

      public:
        virtual ~cpi_base (void);

        virtual void dump (std::string msg = "");
    };

  } // namespace impl

} // namespace saga

#endif //  SAGA_ENGINE_CPIBASE_HPP

