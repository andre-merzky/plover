
#ifndef SAGA_SAGA_ATTRIBUTES_HPP
#define SAGA_SAGA_ATTRIBUTES_HPP

#include <set>
#include <string>

#include <saga/util/shared_ptr.hpp>

namespace saga
{
  class attributes 
  {
    friend class impl::attributes;

    private:
    saga::util::shared_ptr <saga::impl::attributes> attr_impl_;

    protected:
      attributes  (std::set <std::string> ro_s, 
                   std::set <std::string> ro_v, 
                   std::set <std::string> rw_s, 
                   std::set <std::string> rw_v, 
                   bool             extensible);

      // copy c'tor (shallow copy)
      attributes (const attributes & src);

      // copy c'tor (from impl, for inheritance)
      attributes (impl::attributes * impl);
      attributes (saga::util::shared_ptr <impl::attributes> impl);

      ~attributes (void);

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

    protected:
      // friends can see the impl_
      saga::util::shared_ptr <impl::attributes> get_attr_impl (void) const;
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
      // add async attribs
  };
}

#endif // SAGA_SAGA_ATTRIBUTES_HPP

