
#include <set>
#include <string>

#include <saga/impl/core/context.hpp>
#include <saga/impl/core/object.hpp>

#include <saga/saga/core/context.hpp>

saga::impl::context::context  (std::set <std::string> attrib_ro_s, 
                               std::set <std::string> attrib_ro_v, 
                               std::set <std::string> attrib_rw_s, 
                               std::set <std::string> attrib_rw_v, 
                               bool                   extensible)
  : object     (saga::object::Context),
    attributes (attrib_ro_s, 
                attrib_ro_v, 
                attrib_rw_s, 
                attrib_rw_v, 
                extensible)
{
}

// saga::context saga::impl::context::clone (void) const
// {
//   saga::impl::context * tgt = new saga::impl::context ();
//   
//   // copy all contexts to new context
// 
//   return saga::context (tgt);
// }


