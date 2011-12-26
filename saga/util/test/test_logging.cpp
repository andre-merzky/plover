
#include <iostream>

#include <saga/util/logging.hpp>


int main ()
{
  saga::util::enums_dump ();

  saga::util::log (saga::util::Warning,  "one two",             "msg 1 2");
  saga::util::log (saga::util::Error  ,  "two three",           "msg 2 3");
  saga::util::log (saga::util::Critical, "three four five",     "msg 3 4 5");
  saga::util::log (saga::util::Info   ,  "three four five six", "msg 3 4 5 6");

  return 0;
}

