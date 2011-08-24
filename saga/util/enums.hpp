
#ifndef SAGA_UTIL_ENUMS_HPP
#define SAGA_UTIL_ENUMS_HPP

#include <map>

#define REGISTER_ENUM(TYPE,KEY,VAL) \
    static saga::util::enum_register <TYPE> enum_ ## TYPE ## _ ## VAL ## _ (#KEY, static_cast <TYPE> (VAL))


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
          // FIXME: check exists, return "InvalidEnum" otherwise
          return val_map_[typeid (T).name ()][val];
        }

        template <typename T>
        T to_val (const char * key)
        {
          // FIXME: check exists, return "InvalidEnum" otherwise
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
  }
}


#endif // SAGA_UTIL_ENUMS_HPP

