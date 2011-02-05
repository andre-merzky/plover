
#include "pimpl.saga.hpp"


int main ()
{
  // std::cout << "=======================================================\n";
  // {
  //   saga::impl::ref <saga::impl::object> ref_1 (new saga::impl::object ());
  //   saga::impl::ref <saga::impl::object> ref_2 (ref_1);
  //   saga::impl::ref <saga::impl::object> ref_3 = ref_2;

  //   ref_3 = ref_1;

  //   {
  //     saga::impl::ref <saga::impl::object> ref_4 (ref_3);
  //     saga::impl::ref <saga::impl::object> ref_5 = ref_4;
  //     ref_3 = ref_5;
  //   }
  // }
  // std::cout << "=======================================================\n";
  // {
  //   saga::impl::ref <saga::impl::ns_dir> ref_1 (new saga::impl::ns_dir ());
  //   saga::impl::ref <saga::impl::ns_dir> ref_2 (ref_1);
  //   saga::impl::ref <saga::impl::ns_dir> ref_3 = ref_2;

  //   ref_3 = ref_1;

  //   {
  //     saga::impl::ref <saga::impl::ns_dir> ref_4 (ref_3);
  //     saga::impl::ref <saga::impl::ns_dir> ref_5 = ref_4;
  //     ref_3 = ref_5;
  //   }
  // }
  std::cout << "=======================================================\n";

  try
  {
    std::cout << "================== 1\n";
    {
      std::cout << " === object\n";      saga::object      b_a; b_a.check ();
    }
    std::cout << "================== 2\n";
    // std::cout << " === attributes\n";  saga::attributes  b_b; b_b.check ();
    // std::cout << " === monitorable\n"; saga::monitorable b_c; b_c.check ();

    std::cout << "================== 3\n";
    {
      std::cout << " === replica_dir\n";   saga::replica_dir   r_a;   r_a.check ();
    }
    std::cout << "================== 4\n";
    // std::cout << " === ns_file\n";       saga::ns_file       d_ab;  d_ab.check ();
    // std::cout << " === ns_pipe\n";       saga::ns_pipe       d_abc; d_abc.check ();
  }
  catch ( const std::exception & e ) 
  {
    std::cerr << "catched: " << e.what () << "\n";
  }

  return 0;
}

