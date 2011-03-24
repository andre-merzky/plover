
#include "cpi_test.hpp"

int main ()
{
  try
  {
    saga::file f;
    std::cout << "size: " << f.get_size () << std::endl;
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

