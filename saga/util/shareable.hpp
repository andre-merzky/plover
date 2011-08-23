
#ifndef SAGA_UTIL_SHAREABLE_HPP
#define SAGA_UTIL_SHAREABLE_HPP

#include <stdlib.h> // FIXME, only for abort

#include <saga/util/mutex.hpp>
#include <saga/util/shared_ptr.hpp>

namespace saga
{
  namespace util
  {
    //////////////////////////////////////////////////////////////////
    //
    // The shareable class should be inherited from all classes which 
    // want to be able to create shared pointers on their own instance.
    // Note that the classes can so also shoot themself in the foot, e.g. 
    // by creating the first shared ptr on themselves and destroying it 
    // again - bam!
    class shareable
    {
      private:
        bool                valid_;
        saga::util::mutex * mtx_;
        long              * cnt_;

        // the following two functions are to be called from 
        template <typename T> 
        friend class saga::util::shared_ptr;

        void share (saga::util::mutex * mtx,
                    long              * cnt);
        void unshare (void);

        // we want polymorphism, so that dynamic casts work up and down the class
        // hierarchy.  Thus we add one virtual method.
        // FIXME: copied code from saga/core/object.hpp
        virtual bool is_polymorphic (void) { return true; }



      protected:
        // we do not allow direct creation of this class - only inherited
        // classes can use the c'tor
        shareable (void) 
          : valid_ (false)
        {
        }

      public:

        // calling this method in the shared_pre c'tors allows to catch
        // shared_ptr on non-shareable objects on compile time
        bool is_shareable (void);
        bool is_shared    (void);


        // FIXME: for the use cases we know so far (i.e. to create a shared_ptr
        // for *this*), it would suffice to make this method protected...
        template <class T>
        saga::util::shared_ptr <T> shared_this (void)
        {
          // if *this* was part of some shared_ptr before
          if ( valid_ )
          {
            // std::cout << " --- valid " << std::endl;
            // re-use the mutex and counter created by some other shared ptr
            saga::util::shared_ptr <saga::util::shareable> ret (mtx_, cnt_, dynamic_cast <saga::util::shareable*> (this));
            
            return ret.get_shared_ptr <T> ();
          }
          else
          {
            // std::cout << " --- invalid : " << this << std::flush << std::endl;

            // in principle, we could now create a new shared_ptr for *this*, and
            // return it - but the likelyhood is that the caller at some point
            // drops the shared pointer, causing havoc to the original pointer.  
            // So we do not allow this, but only allows shared_this if
            // *this* was shared before (is valid_).

            abort ();
            throw ("cannot get shared_ptr for unshared object");

            // // if not, create new shared ptr, which creates a new mutex and counter
            // saga::util::shared_ptr <T> ret = saga::util::shared_ptr <T> (dynamic_cast <T*> (this));

            // // this new shared_ptr will now register it's mutex and 
            // // counter with *this* instance, and we are good to return.
            // return ret;
          }
        }

        // get the typename of the stored pointer
        std::string get_ptype (void);

        // get the typename of the stored pointer, demangled
        std::string get_ptype_demangled (void);

        saga::util::mutex * get_mutex ()
        {
          if ( ! valid_ )
            throw "Cannot call get_mutex() from unshared instance";

          return mtx_;
        }
    };

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_SHAREABLE_HPP

