
#ifndef SAGA_IMPL_ATTRIBUTES_H
#define SAGA_IMPL_ATTRIBUTES_H

#include <map>
#include <set>
#include <string>

#include <saga/saga/core/attributes.hpp>

namespace saga
{
  namespace impl
  {
    class attributes 
    {
      protected:
        typedef std::map  <std::string, std::string>             attrib_set_s_type;
        typedef std::map  <std::string, std::set <std::string> > attrib_set_v_type;

      private:
        std::set <std::string> ro_s_names_;
        std::set <std::string> ro_v_names_;
        std::set <std::string> rw_s_names_;
        std::set <std::string> rw_v_names_;
        bool                    extensible_;

        attrib_set_s_type attribs_s_;
        attrib_set_v_type attribs_v_;


      public:
        attributes  (std::set <std::string> ro_s_names, 
                     std::set <std::string> ro_v_names, 
                     std::set <std::string> rw_s_names, 
                     std::set <std::string> rw_v_names, 
                     bool             extensible);
        ~attributes (void);

        std::string get_attribute             (std::string key);
        void        set_attribute             (std::string key, 
                                               std::string value);

        std::set <std::string> 
                    get_vector_attribute      (std::string key);
        void        set_vector_attribute      (std::string key, 
                                               std::set <std::string> value);

        bool        attribute_exists          (std::string key,
                                               bool        except = false);
        bool        attribute_is_readonly     (std::string key,
                                               bool        except = true);
        bool        attribute_is_writable     (std::string key,
                                               bool        except = true);
        bool        attribute_is_scalar       (std::string key,
                                               bool        except = true);
        bool        attribute_is_vector       (std::string key,
                                               bool        except = true);
    };

    class attributes_async : public attributes
    {
      public:
        attributes_async  (std::set <std::string> ro_s_names, 
                           std::set <std::string> ro_v_names, 
                           std::set <std::string> rw_s_names, 
                           std::set <std::string> rw_v_names, 
                           bool                    extensible);
        ~attributes_async (void);

        // add async attribs
    };
  }
}

#endif // SAGA_IMPL_ATTRIBUTES_H

