
#include <iostream>

#include <boost/assign/list_of.hpp>

#include <saga/impl/core/attributes.hpp>

#include <saga/saga/core/exception.hpp>

namespace saga 
{
  namespace impl
  {
    attributes::attributes (std::set <std::string> ro_s_names, 
                            std::set <std::string> ro_v_names, 
                            std::set <std::string> rw_s_names, 
                            std::set <std::string> rw_v_names, 
                            bool                   extensible)
      : ro_s_names_ (ro_s_names),
        ro_v_names_ (ro_v_names),
        rw_s_names_ (rw_s_names),
        rw_v_names_ (rw_v_names),
        extensible_ (extensible)
        // make sure that names are not in two lists at once!
    {
      std::set <std::string>::iterator iter;
      
      for ( iter  = ro_s_names_.begin ();
            iter != ro_s_names_.end   (); ++iter )
      {
        attribs_s_[*iter] = "";
      }
      
      for ( iter  = rw_s_names_.begin ();
            iter != rw_s_names_.end   (); ++iter )
      {
        attribs_s_[*iter] = "";
      }
      
      for ( iter  = ro_v_names_.begin ();
            iter != ro_v_names_.end   (); ++iter )
      {
        std::set <std::string> tmp;  
        attribs_v_[*iter] = tmp;
      }
      
      for ( iter  = rw_v_names_.begin ();
            iter != rw_v_names_.end   (); ++iter )
      {
        std::set <std::string> tmp;  
        attribs_v_[*iter] = tmp;
      }
    }

    attributes::~attributes (void)
    {
    }

    std::string attributes::get_attribute (std::string key)
    {
      if ( ! attribute_exists (key) )
      {
        throw saga::DoesNotExist (key + ": attribute does not exist");
      }

      if ( attribute_is_vector (key) )
      {
        throw saga::IncorrectState (key + ": is vector attribute");
      }

      // all checks ok, read attrib
      return attribs_s_[key];
    }

    void attributes::set_attribute (std::string key, 
                                    std::string val)
    {
      if ( ! extensible_ &&
           ! attribute_exists (key, false) )
      {
        throw saga::DoesNotExist (key + ": attribute does not exist");
      }

      if ( attribute_exists    (key, false) &&
           attribute_is_vector (key, false) )
      {
        throw saga::IncorrectState (key + ": is vector attribute");
      }

      if ( attribute_exists      (key, false) &&
           attribute_is_readonly (key, false) )
      {
        throw saga::ReadOnly (key + ": attribute is readonly");
      }

      // all checks ok, set attrib
      attribs_s_[key] = val;
    }


    std::set <std::string> attributes::get_vector_attribute (std::string key)
    {
      if ( ! attribute_exists (key) )
      {
        throw saga::DoesNotExist (key + ": attribute does not exist");
      }

      if ( attribute_is_scalar (key, false) )
      {
        throw saga::IncorrectState (key + ": is scalar attribute");
      }

      // all checks ok, read attrib
      return attribs_v_[key];
    }

    void attributes::set_vector_attribute (std::string key, 
                                           std::set <std::string> val)
    {
      if ( ! extensible_ &&
           ! attribute_exists (key) )
      {
        throw saga::DoesNotExist (key + ": attribute does not exist");
      }

      if ( attribute_exists    (key, false) &&
           attribute_is_scalar (key, false) )
      {
        throw saga::IncorrectState (key + ": is scalar attribute");
      }

      if ( attribute_exists      (key, false) &&
           attribute_is_readonly (key, false) )
      {
        throw saga::ReadOnly (key + ": attribute is readonly");
      }

      // all checks ok, set attrib
      attribs_v_[key] = val;
    }


    bool attributes::attribute_exists (std::string key,
                                       bool        except)
    {
      if ( attribs_s_.find (key) != attribs_s_.end () ||
           attribs_v_.find (key) != attribs_v_.end () ) 
      {
        return true;
      }

      if ( except )
      {
        throw saga::DoesNotExist (key + ": attribute does not exist");
      }

      return false;
    }

    bool attributes::attribute_is_readonly (std::string key, 
                                            bool        except)
    {
      if ( ! extensible_ )
      {
        if ( ! attribute_exists (key, except) )
        {
          if ( except )
          {
            throw saga::DoesNotExist (key + ": attribute does not exist");
          }
        }
      }

      if ( ro_s_names_.find (key) == ro_s_names_.end () &&
           ro_v_names_.find (key) == ro_v_names_.end () ) 
      {
        return false;
      }

      if ( extensible_ )
      {
        return false;
      }

      return true;
    }

    bool attributes::attribute_is_writable (std::string key, 
                                            bool        except)
    {
      if ( ! extensible_ )
      {
        if ( ! attribute_exists (key, except) )
        {
          if ( except )
          {
            throw saga::DoesNotExist (key + ": attribute does not exist");
          }
        }
      }

      if ( rw_s_names_.find (key) == rw_s_names_.end () &&
           rw_v_names_.find (key) == rw_v_names_.end () ) 
      {
        if ( ! extensible_ )
        {
          return false;
        }
      }

      return true;
    }

    bool attributes::attribute_is_scalar (std::string key, 
                                          bool        except)
    {
      if ( ! attribute_exists (key, except) )
      {
        if ( except )
        {
          throw saga::DoesNotExist (key + ": attribute does not exist");
        }
      }

      if ( attribs_s_.find (key) != attribs_s_.end () )
      {
        return true;
      }

      if ( ro_s_names_.find (key) == ro_s_names_.end () &&
           rw_s_names_.find (key) == rw_s_names_.end () ) 
      {
        if ( extensible_ )
        {
          // FIXME: check
          // if we can expand, and key is not taken bys vector yet, then return
          // true: it can be a scalar attrib
          if ( ro_v_names_.find (key) == ro_v_names_.end () &&
               rw_v_names_.find (key) == rw_v_names_.end () &&
                attribs_v_.find (key) ==  attribs_v_.end () )
          {
            return true;
          }

          return false;
        }
      }

      return true;
    }

    bool attributes::attribute_is_vector (std::string key, 
                                          bool        except)
    {
      if ( ! attribute_exists (key, except) )
      {
        if ( except )
        {
          throw saga::DoesNotExist (key + ": attribute does not exist");
        }
      }

      if ( attribs_v_.find (key) != attribs_v_.end () )
      {
        return true;
      }

      if ( ro_v_names_.find (key) == ro_v_names_.end () &&
           rw_v_names_.find (key) == rw_v_names_.end () ) 
      {
        if ( extensible_ )
        {
          // FIXME: check
          // if we can expand, and key is not taken by scalars yet, then return
          // true: it can be a vector attrib
          if ( ro_s_names_.find (key) == ro_s_names_.end () &&
               rw_s_names_.find (key) == rw_s_names_.end () &&
                attribs_s_.find (key) ==  attribs_s_.end () )
          {
            return true;
          }

          return false;
        }
      }

      return true;
    }

  } // ns impl
} // ns saga

