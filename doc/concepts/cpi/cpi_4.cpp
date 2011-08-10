
#include "cpi_3.hpp"

int main ()
{
  try
  {
    saga::file f ("/etc/passwd");
    std::cout << "size: " << f.get_size () << std::endl;
    f.copy ("/tmp/passwd.bak");
  }
  catch ( const std::exception & e )
  {
    std::cerr << "exception: " << e.what () << std::endl;
    return 1;
  }
  catch ( const char * err )
  {
    std::cerr << "error: " << err << std::endl;
    return 1;
  }

  return 0;
}

