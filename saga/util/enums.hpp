
#ifndef SAGA_UTIL_ENUMS_HPP
#define SAGA_UTIL_ENUMS_HPP

#include <map>

namespace saga
{
  namespace util
  {
    class enums
    {
      private:
        std::map <const char *, std::map <const char *, int> > key_map_;
        std::map <const char *, std::map <int, const char *> > val_map_;


      public:
        template <typename T>
        void add (const char* key, T val)
        {
          key_map_[typeid (T).name ()][key] = val;
          val_map_[typeid (T).name ()][val] = key;
        }

        template <typename T>
        const char * to_key (T val)
        {
          const char * type = typeid (T).name ();

          if ( val_map_.find (type) == val_map_.end () )
          {
            return "InvalidEnumType";
          }

          if ( val_map_[type].find (val) == val_map_[type].end () )
          {
            return "InvalidEnum";
          }

          return val_map_[type][val];
        }

        template <typename T>
        T to_val (const char * key)
        {
          const char * type = typeid (T).name ();

          if ( val_map_.find (type) == val_map_.end () )
          {
            return dynamic_cast <T> (-1); 
          }

          if ( val_map_[type].find (key) == val_map_[type].end () )
          {
            return dynamic_cast <T> (-1); 
          }

          return key_map_[typeid (T).name ()][key];
        }
    };

    static enums saga_enums;

    template <typename T>
    class enum_register
    {
      public:
        enum_register (const char * key, T val)
        {
          // std::cout << "register " << saga::util::demangle (typeid (T).name ()) << " :: " << key << std::endl;
          saga_enums.add <T> (key, val);
        }
    };

#define SAGA_UTIL_REGISTER_ENUM(TYPE,KEY,VAL) \
    static saga::util::enum_register <TYPE> enum_ ## TYPE ## _ ## VAL ## _ (#KEY, static_cast <TYPE> (VAL))


  } // namespace util

} // namespace saga


#endif // SAGA_UTIL_ENUMS_HPP

