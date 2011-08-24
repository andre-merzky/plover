
#include <saga/util/shareable.hpp>
#include <saga/util/shared_ptr.hpp>

namespace saga
{
  namespace util
  {
    void shareable::share (saga::util::mutex * mtx,
                           long              * cnt) 
    { 
      mtx_   = mtx; 
      cnt_   = cnt; 
      valid_ = true;
      // std::cout << " --- sharing : " << get_ptype_demangled() << " - " << this << " - " << mtx_ << " - " << cnt_ << std::endl;
    }

    void shareable::unshare (void)
    {
      valid_ = false;
      // std::cout << " --- unsharing : " << get_ptype_demangled() << " - " << this << " - " << mtx_ << " - " << cnt_ << std::endl;
    }

    // calling this method in the shared_pre c'tors allows to catch
    // shared_ptr on non-shareable objects on compile time
    bool shareable::is_shareable (void)
    {
      return true;
    }

    bool shareable::is_shared (void)
    {
      return valid_;
    }


    // get the typename of the stored pointer
    std::string shareable::get_ptype (void)
    {
      if ( NULL == this )
        return "NULL";

      return (typeid (*this).name ());
    }

    // get the typename of the stored pointer, demangled
    std::string shareable::get_ptype_demangled (void)
    {
      return (saga::util::demangle (get_ptype ()));
    }

  } // namespace util

} // namespace saga

