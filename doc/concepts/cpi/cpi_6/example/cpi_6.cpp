

#include <saga/saga.hpp>

#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

int main ()
{
  saga::util::stack_tracer::enabled = true;

  SAGA_UTIL_STACKTRACE ();

  try
  {
    // file open, sync
    if ( 1 )
    {
      LOGSTR (INFO, "main out") << " -sync dir ops #########"  << std::endl;
    
      saga::filesystem::dir d ("/etc/");
      LOGSTR (INFO, "main out") << "dir url: " << d.get_url () << std::endl;
    
      saga::filesystem::file f = d.open ("passwd");
      LOGSTR (INFO, "main out") << "file size: " << f.get_size () << std::endl;
    
      LOGSTR (INFO, "main out") << " #######################"  << std::endl;
    }


    // file copy and get size, sync
    if ( 1 )
    {
      LOGSTR (INFO, "main out") << " -sync file ops ########"  << std::endl;
    
      saga::filesystem::file f ("/etc/passwd");
    
      LOGSTR (INFO, "main out") << "file size: " << f.get_size () << std::endl;
    
      f.copy ("/tmp/passwd.bak");
    
      LOGSTR (INFO, "main out") << " #######################"  << std::endl;
    }


    // file get_size, async
    if ( 1 )
    {
      LOGSTR (INFO, "main out") << " 0 # async get_size ###########################################"  << std::endl;
      
      saga::filesystem::file f ("/etc/passwd");

      LOGSTR (INFO, "main out") << " 1 ############################################################"  << std::endl;
      
      saga::task t = f.get_size <saga::impl::Async> ();
      
      LOGSTR (INFO, "main out") << " 2 ############################################################"  << std::endl;

      saga::impl::call_state s = t.get_state ();

      while ( s == saga::impl::New     ||
              s == saga::impl::Running )
      {
        LOGSTR (INFO, "main out") << "state: " << saga::util::saga_enum_to_key <saga::impl::call_state> (t.get_state ()) << std::endl;
        ::sleep (1);
        s = t.get_state ();
      }

      LOGSTR (INFO, "main out") << "state: " << saga::util::saga_enum_to_key <saga::impl::call_state> (t.get_state ()) << std::endl;

      LOGSTR (INFO, "main out") << " 3 ############################################################"  << std::endl;

      LOGSTR (INFO, "main out") << "value: " << t.get_result <int> () << std::endl;

      LOGSTR (INFO, "main out") << " 4 ############################################################"  << std::endl;

      ::sleep (2);
    }

  }
  catch ( const std::exception & e )
  {
    std::cerr << "exception: " << e.what () << std::endl;
    return 1;
  }
  catch ( const char * err )
  {
    std::cerr << "error const char *: " << err << std::endl;
    return 1;
  }

  // muntrace();

  return 0;
}

