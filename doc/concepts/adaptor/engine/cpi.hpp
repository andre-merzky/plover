
#ifndef SAGA_ADAPTOR_CPI_HPP
#define SAGA_ADAPTOR_CPI_HPP

namespace saga
{
  namespace cpi
  {
    class object
    {
      public:
        object (void) {};
        virtual ~object (void) {};

        // MUST overload init
        virtual void init (int i) { 
          throw "saga::exception::not_implemented::virtual_method object::init";
        }

        // call works ok:  return true
        // call fails:     throw exception
        // call is filter: return false
        //
        // filter:  a filter adaptor implements calls which do not provide the
        // full call semantics.  instead, they are able to do parameter checking,
        // or preprocessing of parameters, etc.  So if a call returns 'false', it
        // basically signals NotImplemented - try next adaptor.
        //
        // FIXME: solve the problem of calls which need to collect r esults over
        // multiple adaptors -- when do we then invoke filter calls?  That needs
        // support on adaptor scheduling level...
        //
        virtual bool get_id (int & i) 
        {
          throw "saga::exception::not_implemented::virtual_method";
        }
    };

    class context : public saga::cpi::object
    {
      public:
        context (void) {};
        virtual ~context (void) {};

        virtual bool get_sqrid (std::string & sqrid) 
        {
          throw "saga::exception::not_implemented::virtual_method";
        }
    };

  } // namspace cpi

} // namespace saga

#endif // SAGA_ADAPTOR_CPI_HPP

