
#include <iostream>

#include <saga/util/logging.hpp>

#define LOG       saga::util::log

#define NOISE     saga::util::logging::Noise    
#define DEBUG     saga::util::logging::Debug    
#define INFO      saga::util::logging::Info     
#define NOTICE    saga::util::logging::Notice   
#define WARNING   saga::util::logging::Warning  
#define ERROR     saga::util::logging::Error    
#define CRITICAL  saga::util::logging::Critical 
#define ALERT     saga::util::logging::Alert    
#define EMERGENCY saga::util::logging::Emergency

int main ()
{
  saga::util::enums_dump ();

  saga::util::log (saga::util::logging::Warning,  "one two",             "msg 1 2");
  saga::util::log (saga::util::logging::Error  ,  "two three",           "msg 2 3");
  saga::util::log (saga::util::logging::Critical, "three four five",     "msg 3 4 5");
  saga::util::log (saga::util::logging::Info   ,  "three four five six", "msg 3 4 5 6");

  LOG (WARNING,  "one two",             "msg 1 2");
  LOG (ERROR  ,  "two three",           "msg 2 3");
  LOG (CRITICAL, "three four five",     "msg 3 4 5");
  LOG (INFO   ,  "three four five six", "msg 3 4 5 6");

  return 0;
}

