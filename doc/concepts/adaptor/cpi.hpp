
#ifndef ADAPTOR_CPI_HPP
#define ADAPTOR_CPI_HPP

namespace cpi
{
  class object
  {
    public:
      object (void) {};
      virtual ~object (void) {};

      // MUST overload init
      virtual void init (void) = 0;
      
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
      virtual bool get_id (void) 
      {
        throw "saga::exception::not_implemented::virtual_method";
      }
  };
}

#endif // ADAPTOR_CPI_HPP

