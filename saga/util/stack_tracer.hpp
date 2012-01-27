
#ifndef SAGA_UTIL_STACKTRACER_HPP
#define SAGA_UTIL_STACKTRACER_HPP

#include <iostream>
#include <saga/util/logging.hpp>

namespace saga
{
  namespace util
  {
    class stack_tracer
    {
      // FIXME: we should maintain one stacktrace per thread, obviously
      private:
        static unsigned int indent_;

        std::string  sig_;
        std::string  file_;
        int          line_;
        std::string  msg_;

        std::string indent   (void);
        std::string unindent (void);

        static std::vector <std::string> stack_;


      public:
        static bool enabled;

        stack_tracer (const char * sig, 
                      const char * file, 
                      int          line, 
                      const char * msg = "");
        ~stack_tracer (void);

        static void dump (void);
    };

// //-------------------------------------------------------------
// #define SAGA_UTIL_TYPEID(x) saga::util::type_id::fn (x, #x)   \
// //-------------------------------------------------------------

//-------------------------------------------------------------
#define SAGA_UTIL_STACKDUMP()                                 \
  saga::util::stack_tracer::dump ()                           \
//-------------------------------------------------------------

//-------------------------------------------------------------
#define SAGA_UTIL_STACKTRACE()                                \
  saga::util::stack_tracer                                    \
    saga_util_stack_tracer_instance_ (                        \
      __PRETTY_FUNCTION__, __FILE__, __LINE__)                \
//-------------------------------------------------------------

//-------------------------------------------------------------
#define SAGA_UTIL_STACKTRACE_MSG(MSG)                         \
  saga::util::stack_tracer                                    \
    saga_util_stack_tracer_instance_ (                        \
      __PRETTY_FUNCTION__, __FILE__, __LINE__, MSG)           \
//-------------------------------------------------------------


  } // namespace util

} // namespace saga


#endif // SAGA_UTIL_STACKTRACER_HPP

