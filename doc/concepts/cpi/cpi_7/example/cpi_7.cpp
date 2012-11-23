

#include <saga/saga.hpp>

#include <saga/util/logging.hpp>
#include <saga/util/stack_tracer.hpp>

int main ()
{
  saga::util::stack_tracer::enabled = true;

  saga::util::logstr (saga::util::logging::Warning, "") <<  "log";

  SAGA_UTIL_STACKTRACE ();

  try
  {
    // file open, sync
    if ( 0 )
    {
      LOGSTR (INFO, "main out") << "-sync dir ops "  << std::endl;
    
      saga::filesystem::dir  d ("/etc/");
      saga::filesystem::file f1 = d.open ("passwd");
      saga::filesystem::file f2 = d.open ("group");

      LOGSTR (INFO, "main out") << "dir url: " << d.get_url () << std::endl;
    
      for ( int i = 0; i < 1000; i++ )
      {
        std::cout << "." << std::flush;
        size_t size1 = f1.get_size ();
      }
      std::cout << std::endl;

      LOGSTR (INFO, "main out") << "file2 size: " << f2.get_size () << std::endl;
    }

    // file open, async
    if ( 1 )
    {
      LOGSTR (INFO, "main out") << "async dir ops "  << std::endl;
    
      saga::filesystem::dir  d ("/etc/");
      saga::filesystem::file f1 = d.open ("passwd");
      saga::filesystem::file f2 = d.open ("group");

      LOGSTR (INFO, "main out") << "dir url: " << d.get_url () << std::endl;

      std::vector <saga::async::task> tasks;
      for ( int i = 0; i < 1000; i++ )
      {
        std::cout << "_" << std::flush;
        saga::async::task t = f1.get_size <saga::async::Async> ();
        t.run ();
        sleep (1);
        tasks.push_back (t);
      }
      std::cout << std::endl;

      for ( int i = 0; i < 1000; i++ )
      {
        std::cout << "-" << std::flush;
        tasks[i].wait ();
        size_t size = tasks[i].get_result <size_t> ();
        std::cout << size << " ";
      }
      std::cout << std::endl;

      LOGSTR (INFO, "main out") << "file2 size: " << f2.get_size () << std::endl;
    }


    // file copy and get size, sync
    if ( 1 )
    {
      LOGSTR (INFO, "main out") << "sync file ops "  << std::endl;
    
      saga::filesystem::file f ("/etc/passwd");
    
      LOGSTR (INFO, "main out") << "file size: " << f.get_size () << std::endl;
    
      f.copy ("/tmp/passwd.bak");

      LOGSTR (INFO, "main out") << "copy done " << std::endl;
    }


    // file get_size, async
    if ( 1 )
    {
      LOGSTR (INFO, "main out") << "0 (a)sync get_size "  << std::endl;
      
      saga::filesystem::file f ("/etc/passwd");

      saga::async::task  t = f.get_size <saga::async::Async> ();
      saga::async::state s = t.get_state ();

      LOGSTR (INFO, "main out") << "state: " << saga::util::saga_enum_to_key <saga::async::state> (t.get_state ()) << std::endl;

      t.wait ();

      LOGSTR (INFO, "main out") << "state: " << saga::util::saga_enum_to_key <saga::async::state> (t.get_state ()) << std::endl;
      LOGSTR (INFO, "main out") << "value: " << t.get_result <size_t> () << std::endl;

    }

    // file get_size, async
    if ( 1 )
    {
      LOGSTR (INFO, "main out") << "5 async get_size "  << std::endl;
      
      saga::filesystem::file f ("/etc/passwd");

      saga::async::task  t = f.get_size <saga::async::Async> ();
      saga::async::state s = t.get_state ();

      LOGSTR (INFO, "main out") << "state: " << saga::util::saga_enum_to_key <saga::async::state> (t.get_state ()) << std::endl;

      t.wait ();

      LOGSTR (INFO, "main out") << "state: " << saga::util::saga_enum_to_key <saga::async::state> (t.get_state ()) << std::endl;
      LOGSTR (INFO, "main out") << "value: " << t.get_result <size_t> () << std::endl;
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

