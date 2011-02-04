
#ifndef SAGA_SAGA_ATTRIBUTES_HPP
#define SAGA_SAGA_ATTRIBUTES_HPP

#include <set>
#include <string>

#include <saga/util/shared_ptr.hpp>

namespace saga
{
  namespace impl 
  {
    class attributes;
  }


  class attributes 
  {
    typedef saga::impl::attributes impl_type;

    friend class saga::impl::attributes;


    private:
      // this is a base class (inherits nothing),
      // thus we have to keep an impl pointer
      saga::util::shared_ptr <impl_type> attr_impl_;

      // this is an interface: d'tor is private and virtual
      virtual ~attributes (void);


    protected:
      // friends can create instances with state
      attributes  (std::set <std::string> ro_s, 
                   std::set <std::string> ro_v, 
                   std::set <std::string> rw_s, 
                   std::set <std::string> rw_v, 
                   bool             extensible);

      // copy c'tor (shallow copy)
      attributes (const attributes & src);

      // copy c'tor (from impl, for inheritance)
      attributes (                        impl_type * impl);
      attributes (saga::util::shared_ptr <impl_type>  impl);

      // friends can see the impl_
      template <class T>
      saga::util::shared_ptr <impl_type> get_attr_impl (void) const;
      template <class T>
      saga::util::shared_ptr <impl_type> get_impl (void) const;


    public:
      std::string get_attribute             (std::string key);
      void        set_attribute             (std::string key, 
                                             std::string value);

      std::set <std::string> 
                  get_vector_attribute      (std::string key);
      void        set_vector_attribute      (std::string key, 
                                             std::set <std::string> value);

      bool        attribute_exists          (std::string key);
      bool        attribute_is_readonly     (std::string key);
      bool        attribute_is_writable     (std::string key);
      bool        attribute_is_scalar       (std::string key);
      bool        attribute_is_vector       (std::string key);
  };

  class attributes_async : public attributes
  {
    protected:
      attributes_async  (std::set <std::string> ro_s, 
                         std::set <std::string> ro_v, 
                         std::set <std::string> rw_s, 
                         std::set <std::string> rw_v, 
                         bool             extensible);

      ~attributes_async (void);

    public:
      // add async attrib methods
  };
}

#endif // SAGA_SAGA_ATTRIBUTES_HPP

