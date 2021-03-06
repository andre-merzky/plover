
#ifndef ADAPTOR_HPP
#define ADAPTOR_HPP

#include <saga/util/shared_ptr.hpp>

#include "../engine/cpi.hpp"                // cpi's to implement
#include "../engine/adaptor_registry.hpp"   // register with engine

// this adaptor implements two cpi classes, object and context.  The
// create_xxx_cpi() methods are factory methods which create instancies 
// of those cpi implementations.  The register_cpi() method is called by 
// the saga engine to register these two factory methods for the respective
// cpi's.

extern "C" 
{
  saga::util::shared_ptr <saga::cpi::object> create_context_cpi (void);
  saga::util::shared_ptr <saga::cpi::object> create_object_cpi  (void);

  void  register_cpi (saga::detail::adaptor_registry & reg);
}


// all cpi object implementations live in a unique namespace.
namespace adaptor_1
{
  class object : public saga::cpi::object
  {
    private:
      int id_;

    public:
      object (void)
        : id_ (0)
      {
        std::cout << "adaptor_1::object::c'tor" << std::endl;
      }

      ~object (void)
      {
        std::cout << "adaptor_1::object::d'tor" << std::endl;
      }

      void init (int id)
      {
        std::cout << "adaptor_1::object::init: " << id << std::endl;
        id_ = id;
      }

      bool get_id (int & id)
      {
        id = id_;

        return true;
      }
  };


  class context : public saga::cpi::context
  {
    private:
      int id_;

    public:
      context (void)
        : id_ (0)
      {
        std::cout << "adaptor_1::context::c'tor" << std::endl;
      }

      ~context (void)
      {
        std::cout << "adaptor_1::context::d'tor" << std::endl;
      }

      void init (int id)
      {
        std::cout << "adaptor_1::context::init: " << id << std::endl;
        id_ = id;
      }

      bool get_sqrid (int & id)
      {
        id = id_ * id_;

        return true;
      }
  };
}

#endif // ADAPTOR_HPP

