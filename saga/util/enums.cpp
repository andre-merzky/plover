
#include <saga/util/logging.hpp>
#include <saga/util/enums.hpp>
#include <saga/util/singleton.hpp>

namespace saga
{
  namespace util
  {
    void enums::dump (void)
    {
      {
        std::map <std::string, std::map <std::string, int> > :: iterator i1;

        for ( i1  = key_map_.begin (); 
              i1 != key_map_.end   ();
              i1++ )
        {
          saga::util::log (saga::util::logging::Info, "enums dump", "  " + i1->first);

          std::map <std::string, int> :: iterator i2;

          for ( i2  = key_map_[i1->first].begin (); 
                i2 != key_map_[i1->first].end   ();
                i2++ )
          {
            std::string msg;

            msg += "    ";
            msg += i2->first;
            msg += " \t";
            msg += i2->second;

            saga::util::log (saga::util::logging::Info, "enums dump", msg);
          }
        }
        saga::util::log (saga::util::logging::Info, "enums dump", "");
      }


      {
        std::map <std::string, std::map <int, std::string> > :: iterator i1;

        for ( i1  = val_map_.begin (); 
              i1 != val_map_.end   ();
              i1++ )
        {
          saga::util::log (saga::util::logging::Info, "enums dump", "  " + i1->first);

          std::map <int, std::string> :: iterator i2;

          for ( i2  = val_map_[i1->first].begin (); 
                i2 != val_map_[i1->first].end   ();
                i2++ )
          {
            std::string msg;

            msg += "    ";
            msg += i2->first;
            msg += " \t";
            msg += i2->second;

            saga::util::log (saga::util::logging::Info, "enums dump", msg);
          }
        }
        saga::util::log (saga::util::logging::Info, "enums dump", "");
      }
    }

    void enums_dump (void)
    {
      saga_enums::get_singleton ()->dump ();
    }

  } // namespace util

} // namespace saga


