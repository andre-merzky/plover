
#ifndef PIMPL_SAGA_HPP
#define PIMPL_SAGA_HPP

#include "pimpl.impl.hpp"

namespace saga
{
  // helper structs
  struct attribute_info_t
  {
    std::string name;
    std::string type;
    std::string desc;
    std::string mode;
    std::string value;
    std::string notes;
  };
  #define ATTRIBUTES_EXTENSIBLE_YES 1
  #define ATTRIBUTES_EXTENSIBLE_NO  0

  struct metric_info_t
  {
    std::string name;
    std::string desc;
    std::string mode;
    std::string unit;
    std::string type;
    std::string value;
    std::string notes;
  };
  #define METRICS_EXTENSIBLE_YES 1
  #define METRICS_EXTENSIBLE_NO  0


  // facade classes
  // 
  // If facade impls are initialized as shown here, always inheriting from
  // saga::object.
  // hold a ref instance.  Although that class is indeed lightweight, it will add up.  So, one
  // _may_ want to consider to use one ref, either here in the top facade class, or in the base
  // class (object), and to cast refs' up or down on get_impl.
  class object
  {
    private:
      saga::impl::ref <saga::impl::object> ref_;

    protected:
      saga::impl::ref <saga::impl::object> get_ref (void) { return ref_; }

    public:
      // create a new impl object, store the ptr in the ref container, and keep that alive.
      object () 
        : ref_ (new saga::impl::object ())
      { 
        std::cout << "c'tor base::object    () \t- " << ref_.get_impl () << "\n";
        check ();
      }

      // On occasion, we get a ref container handed down from child classes.  Keep that contained,
      // and treat like a self created one.
      object (const saga::impl::ref <saga::impl::object> & ref) 
        : ref_ (ref)
      { 
        std::cout << "c'tor base::object    (ref) \t- " << ref_.get_impl () << "\n";
        check ();
      }

      void check ()
      {
        std::cout << "check saga::object    () \t- " << ref_.get_impl () << "\n";
        ref_.get_impl () -> check ();
      }

      ~object () 
      {
        std::cout << "check saga::~object   () \t- " << ref_.get_impl () << "\n";
        // ref_ goes out of scope here, but its impl pointer may life on elsewhere, in other refs
        // to the same impl.
      }
  };


  class ns_dir : public object
  {
    private:
      // we don't actually need ref locally - i just wastes memory.  But we might keep it as cached version.
      // TODO: need to benchmark performance versus memory impact
      saga::impl::ref <saga::impl::ns_dir> ref_;

    public:
      // create a new impl object, store the ptr in the ref container, and keep that alive.  Also
      // create another ref container for the same ptr in all base classes (here only object).
      ns_dir ()  
        : object (saga::impl::create_ref <saga::impl::ns_dir> (new saga::impl::ns_dir ()).get_ref <saga::impl::object> ()),
          // get the ref back from the base class, with correct type
          ref_        (this->saga::object::get_ref ().get_ref <saga::impl::ns_dir>      ())
        
     // why does the following cause a 'expected primary-expression' error (return line of create_ref <T,U> ()(
     // : object (saga::impl::create_ref <saga::impl::ns_dir, saga::impl::object> (new saga::impl::ns_dir ()))
     
      { 
        std::cout << "c'tor base::ns_dir    () \t- " << ref_.get_impl () << "\n";
        check ();
      }

      // On occasion, we get a ref container handed down from child classes.  Keep that contained,
      // and treat like a self created one.
      ns_dir (const saga::impl::ref <saga::impl::ns_dir> & ref) 
        : ref_ (ref) // ,
       // object (ref_.get_ref <saga::impl::object> ())
      { 
        std::cout << "c'tor base::object    (ref) \t- " << ref_.get_impl () << "\n";
        check ();
      }

      void check ()
      {
        std::cout << "check saga::ns_dir    () \t- " << ref_.get_impl () << "\n";
        ref_.get_impl () -> check ();
      }

      ~ns_dir () 
      {
        std::cout << "d'tor saga::~ns_dir   () \t- " << ref_.get_impl () << "\n";
        // ref_ goes out of scope here, but its impl pointer may life on elsewhere, in other refs
        // to the same impl.
      }
  };

  // interfaces, don't inherit anything
  class attributes
  {
    private:
      attribute_info_t * info_;
      bool               ext_;

    public:
      attributes (saga::impl::ref <saga::impl::attribute> ref, 
                  attribute_info_t info[], 
                  bool extensible)
      {
        info_ = info;
        ext_  = extensible;
        std::cout << "c'tor saga::attributes() \t\n";
      }
  };

  class monitorable
  {
    private:
      metric_info_t * info_;
      bool            ext_;

    public:
      monitorable (saga::impl::ref <saga::impl::monitorable> ref, 
                   metric_info_t info[], 
                   bool extensible)
      {
        info_ = info;
        ext_  = extensible;
        std::cout << "c'tor saga::monitorable() \t\n";
      }
  };


#include "replica_attributes.hpp"
#include "replica_metrics.hpp"

  class replica_dir : public ns_dir, 
                      public attributes, 
                      public monitorable
  {
    public:
      replica_dir (void)
          // pass refs to the implemented interfaces, so that get_<interface>_ref() etc works.  
          // FIXME: do we actually need those, since we have the get_ref <interface> already?
        : attribute   (this->saga::object::get_ref ().get_ref <saga::impl::attribute>   (), 
                       replica_dir_attributes, ATTRIBUTES_EXTENSIBLE_YES)
        , monitorable (this->saga::object::get_ref ().get_ref <saga::impl::monitorable> (),
                       replica_dir_metrics, METRICS_EXTENSIBLE_NO)
      {
      }
  };
} // namespace saga

#endif PIMPL_SAGA_HPP

