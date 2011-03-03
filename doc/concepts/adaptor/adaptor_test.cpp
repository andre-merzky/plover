
#include <iostream>

#include "engine/engine.hpp"

//////////////////////////////////////////////////////////////////////
//
// A very central element in SAGA is the notion of adaptors.  An
// adaptor is a component which binds a specific SAGA API call
// to some middleware, and thus fills that call with an
// implementation, and semantics.
//
// Each adaptor implement a set of specific packages and
// objects.  For example, a saga::job adaptor would implement
// the objects from the saga::job package.  For that, the
// adaptor creates classes which inherit the saga::job::cpi
// classes.  Those classes basically reflect the high level SAGA
// API, but only provide virtual methods (they are almost pure
// virtual classes, and thus interfaces).  The adaptor
// implementation can then overload those methods, providing the
// semantics for the syntax defined by the cpi interfaces, and
// thus by the SAGA API.
//
// The SAGA library will load the adaptor library, and create
// cpi object instances from it.  Those are, on library level,
// purely treated as cpi classes, but as those are pure virtual,
// all calls are finally routed to the adaptor implementation.
//
// The SAGA library can load multiple adaptors for each package,
// and can select the actual adaptor instance to be used at
// runtime.  Further, if one adaptor fails, we can continue by
// trying the next adaptor which implements the same cpi
// interface.  In order for that to work, we have to maintain
// some amount of state between individual adaptor invocations.
// For example, a saga::filesystem::file instance needs to keep
// the file's URL, open mode, and file position pointer around
// in order to allow the various adaptor to do 'the right
// thing'.  Those state information are called
// 'object_instance_data', and are maintained by the
// implementation part of our pimpl scheme (see documentation to
// pimpl).
//
// There are some SAGA API calls which require the SAGA library
// to collect results over multiple adapor instances.  For
// example saga::job::service::list() should be able to list job
// IDs from multiple backends.  For such calls, the engine will
// need to run all adaptors in parallel (to reduce latency), and
// combine the results.
//
// Finally, there are adaptors which operate on the call
// parameters etc, but do not implement the full method
// semantics.  An obvious example would be an adaptor which does
// basic parameter checking, but does not perform any backend
// activity.  Those adaptors are called 'filters', and the
// engine will continue to invoke other adaptors after all
// filter adaptors have been successfully used on the call.
//
// adaptors need to register the cpi classes they implement with
// the engine.  That is done by calling
//
//   saga::adaptors::register_cpi (enum   flag, 
//                                 string cpi,
//                                 string factory);
//
// The flag determines the adaptor scheduling.  If it is set to
// saga::adaptors::Filter, the adaptor will always be scheduled
// before non-filter adaptors, exactly once per call, and
// non-filter adaptors will only be invoked if the call to the
// filter cpi suceeded (returns true).
//
// The string 'factory' contains the symbol for a factory
// method, which can be loaded by the engine via dlsym, and
// which then can be used to create cpi method instances.  The
// factory method has the signature (all in namespace saga):
//
//   shared_ptr <cpi> factory (saga::session s);
//
// The session determines what session the resulting object
// should live in.  Note that adaptor and object instance data
// will be available to the cpi, and that global and adaptor
// level preferences (ini) will be part of thye
// adaptor's instance data.
//
// Note that the factory will create a basically empty object
// - the object's c'tor will not be able to receive any
// parameters.  Instead, a call to a cpi specific initialization
// method, object->init (par, ...), will be used to fully
// initialize the object.
//   
// This main() represents the saga engine, so that part of the
// saga library which loads the adaptor libraries, lets the
// adaptors register their cpi implementations, retrieves the
// adaptor's cpi factories, calls those factories, initializes
// the resulting objects, and calls the objects methods.  The
// saga engine is the very core of the SAGA API!
//
int main ()
{
  try
  {
    saga::detail::engine e;

    // load two adaptors
    e.load_adaptor ("adaptor_1/libsaga_adaptor_1.so");
    e.load_adaptor ("adaptor_2/libsaga_adaptor_2.so");

    e.run ();
  }
  catch ( const std::exception & e )
  {
    std::cout << "std::exception " << e.what () << std::endl;
  }
//catch ( const std::string err )
//{
//  std::cout << "error string " << err << std::endl;
//}
//catch ( const char * err )
//{
//  std::cout << "error char* " << err << std::endl;
//}


  return (0);
}

