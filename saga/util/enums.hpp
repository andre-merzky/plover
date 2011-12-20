
#ifndef SAGA_UTIL_ENUMS_HPP
#define SAGA_UTIL_ENUMS_HPP

#include <typeinfo>
#include <string>
#include <iostream>
#include <map>

#include <saga/util/utils.hpp>
#include <saga/util/singleton.hpp>

namespace saga
{
  namespace util
  {
    void enums_dump (void);

    class enums
    {
      private:
        std::map <std::string, std::map <std::string, int        > > key_map_;
        std::map <std::string, std::map <int,         std::string> > val_map_;

        template <typename T>
        std::string type_ (void)
        {
          std::string s = typeid (T).name ();

          for ( unsigned int i = 0; i < s.size (); i++ )
          {
            if ( s[i] == ':' )
            {
              s[i] = '_';
            }
          }

          return s;
        }


      public:
        enums (void)
        {
        }

        ~enums (void)
        {
        }


        template <typename T>
        void add (std::string const & key, T val)
        {
          std::string t (type_ <T> ());
          std::string k (key);
          int         v (val);

          key_map_[t][k] = v;
          val_map_[t][v] = k;
        }


        template <typename T>
        std::string to_key (T val)
        {
          std::string t (type_ <T> ());
          int         v (val);

          if ( val_map_.find (t) == val_map_.end () )
          {
            throw "InvalidEnumType";
          }

          if ( val_map_[t].find (v) == val_map_[t].end () )
          {
            throw "InvalidEnum";
          }

          return val_map_[t][v];
        }


        template <typename T>
        T to_val (const char * key, 
                  bool         icase = false)  // ignore case
        {
          std::string type = type_ <T> ();

          if ( key_map_.find (type) == key_map_.end () )
          {
            std::cerr << "no registered enums of type '" << type_ <T> () << "'" << std::endl;
            throw "cannot convert enum 1";
          }

          // case insensitive: we can do a direct map lookup
          if ( ! icase )
          {
            if ( key_map_[type].find (key) == key_map_[type].end () )
            {
              std::cerr << "cannot convert '" << key << "' to enum '" << type_ <T> () << "'" << std::endl;
              throw "cannot convert enum 2";
            }

            return T (key_map_[type][key]);
          }
          else
          {
            // ignore case - we have to do the map lookup manually
            // NOTE: this is costly, use with care
            // NOTE: we expect the type string to be case sensitive still, as it
            //       is generated locally
            std::map <std::string, int> e_map = key_map_[type];
            std::map <std::string, int> :: iterator it;

            for ( it  = e_map.begin (); 
                  it != e_map.end   (); 
                  it++ )
            {
              if ( saga::util::icompare (it->first, key) )
              {
                return T (it->second);
              }    
            }
          }


          // Ooops!
          std::cerr << "cannot convert '" << key << "' to enum '" 
                    << type_ <T> () << "' - not sure what happened" << std::endl;
          throw "cannot convert enum 3";
        }


        void dump (void)
        {
          {
            std::map <std::string, std::map <std::string, int> > :: iterator i1;

            for ( i1  = key_map_.begin (); 
                  i1 != key_map_.end   ();
                  i1++ )
            {
              std::cout << "  " << i1->first << std::endl;

              std::map <std::string, int> :: iterator i2;

              for ( i2  = key_map_[i1->first].begin (); 
                    i2 != key_map_[i1->first].end   ();
                    i2++ )
              {
                std::cout << "    " << i2->first << " \t" << i2->second << std::endl;
              }
            }
            std::cout << std::endl;
          }


          {
            std::map <std::string, std::map <int, std::string> > :: iterator i1;

            for ( i1  = val_map_.begin (); 
                  i1 != val_map_.end   ();
                  i1++ )
            {
              std::cout << "  " << i1->first << std::endl;

              std::map <int, std::string> :: iterator i2;

              for ( i2  = val_map_[i1->first].begin (); 
                    i2 != val_map_[i1->first].end   ();
                    i2++ )
              {
                std::cout << "    " << i2->first << " \t" << i2->second << std::endl;
              }
            }
            std::cout << std::endl;
          }
        }
    };

    typedef singleton <enums> saga_enums;

    template <typename T>
    class enum_register
    {
      public:
        enum_register (const char * key, T val)
        {
          saga_enums::get_singleton ()->add <T> (key, val);
        }
    };


    // we allow for two types of macros - the second one allows "::" in the TYPE
    // name, but then needs a string identifier where :: is replaced by __.
    // Somewhat obscure, but that is what we use 'til we have a better scheme...
    // That allows to define enums in a class definition, and to register them
    // outside of that definition.  The price we pay is that we need to replace
    // :: with __ in type names...
#define SAGA_UTIL_REGISTER_ENUM(TYPE,KEY,VAL) \
    static saga::util::enum_register <TYPE> enum_ ## TYPE ## _ ## VAL ## _ (#KEY, static_cast <TYPE> (VAL))
#define SAGA_UTIL_REGISTER_ENUM_S(TYPE,TYPES,KEY,VAL) \
    static saga::util::enum_register <TYPE> enum_ ## TYPES ## _ ## VAL ## _ (#KEY, static_cast <TYPE> (VAL))


  } // namespace util

} // namespace saga


#endif // SAGA_UTIL_ENUMS_HPP

