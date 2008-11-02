
#ifndef SAGA_SAGA_OBJECT_HPP
#define SAGA_SAGA_OBJECT_HPP

#include <string> 

#include <saga/util/shared_ptr.hpp>

namespace saga
{
  class object
  {
    friend class impl::object;

    public:
      // The SAGA object type enum allows for inspection of 
      // SAGA object instances.  
      enum type
      {
        Unknown              = -1,
        Exception            =  1,
        Session              =  2,
        Context              =  3,
        Task                 =  4,
        TaskContainer        =  5,
        Buffer               =  6,
        Metric               =  7,
        NSEntry              =  8,
        NSDirectory          =  9,
        IOVec                = 10,
        File                 = 11,
        Directory            = 12,
        LogicalFile          = 13,
        LogicalDirectory     = 14,
        JobDescription       = 15,
        JobServer            = 16,
        Job                  = 17,
        StreamServer         = 18,
        Stream               = 19,
        RPC                  = 20,
        Parameter            = 21
      };

    private:
      // object implementation
      saga::util::shared_ptr <impl::object> obj_impl_;

      // remember if session was set
      bool has_session_;

      // store shallow copy of objects session, and keep it alive for the
      // objects lifetime.
      saga::util::shared_ptr <impl::session> session_impl_;

      // we want polymorphism, so that dynamic casts work up and down the class
      // hierarchy.  Thus we add one virtual method.
      virtual bool is_polymorphic (void) { return true; }


    public:

      // inspection methods

      // return a globally unique object id (formatted as UUID).
      virtual
      std::string get_id      (void) const;

      // return the appropriate object_type enum
      virtual
      type        get_type    (void) const;

      // return the session that object instance was created in, 
      // or throw DoesNotExist
      virtual
      session     get_session (void) const;


      // memory management

      // allow for deep copy of object instances (shallow copy is the default)
      virtual 
      object      clone       (void) const;


    protected:
      // object is an interface, thus c'tors and d'tors are protected

      // objects w/o session.
      object (type t);

      // objects w/ session.
      object (type t, saga::session session);

      // copy c'tor (shallow copy)
      object (const object & src);

      // copy c'tor (from impl, for inheritance)
      object (impl::object * impl);

      // copy c'tor (from shared impl, for inheritance)
      object (saga::util::shared_ptr <impl::object> impl);

      // d'tor is virtual, to allow safe destruction after casting
      virtual ~object (void);


      // friends can see the obj_impl_
      saga::util::shared_ptr <saga::impl::object> get_obj_impl (void) const;
  };
}

#endif // SAGA_SAGA_OBJECT_HPP

