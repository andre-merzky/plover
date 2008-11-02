
#ifndef SAGA_SAGA_FILE_HPP
#define SAGA_SAGA_FILE_HPP

#include <saga/util/shared_ptr.hpp>

#include <saga/saga/object.hpp>

namespace saga
{
  class file : public object
  {
    friend class saga::impl::file;
    friend class saga::impl::object;

    public:
      // default file impl
      static saga::impl::file * none_;
      static saga::impl::file * default_;

      // create file from impl
      file (saga::impl::file * impl);

      file  (void);
      ~file (void);

      // copy c'tor
      file  (const saga::file & src);

      static file no_file (void);
      static file default_file (void);

    protected:
      // get impl from base class
      saga::util::shared_ptr <saga::impl::object> get_obj_impl (void) const;

      // c'tor from impl
      file (saga::util::shared_ptr <saga::impl::file> impl);
  };
}


#endif // SAGA_SAGA_FILE_HPP

