
namespace saga
{
  //////////////////////////////////////////////////////////////////////
  //
  // core functionality which is *not* part of API nor package
  //
  namespace impl
  {
    class engine;  // loads adaptors, coordinates cpi calling
    class functor; // encapsulates function call
  }

  //////////////////////////////////////////////////////////////////////
  //
  // SAGA API
  //
  namespace util // exposed in API!
  {
    class mutex;
    class scoped_lock;
    class shared_counter;
    class shared_ptr;
    class assign;
    string demangle (string);
    // ...
  }

  class object       // base object class
  {
    bool is_a <T> (void); // inspection
    T    get  <T> (void); // casting
  }

  class attributes;  // interface
  class monitorable; // interface

  namespace name_space
  {
    class entry // holds pointer to impl
    {
      class impl // checks basic semantics, manages state, talks to cpis
      {
        class state; // == instance data
        class cpi;   // virtual class, adaptor base class (cpi implementation == adaptor)
      }
    };
  }

  namespace filesystem
  {
    enum  flags;
    class file;      // holds pointer to impl
    class directory; // holds pointer to impl
  }

  //////////////////////////////////////////////////////////////////////
  //
  // separate namespace for adaptors, not exposed as API
  // (should go into impl?)
  //
  namespace adaptor // classes below here implement a or multiple cpis
  {
    namespace local // adaptor suite
    {
      class object;
      class attributes;
      class monitorable;
      
      namespace name_space
      {
        class entry;
        class directory;
      }

      namespace filesystem
      {
        class file;
        class directory;
      }
    }
  }
}

