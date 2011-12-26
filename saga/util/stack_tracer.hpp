
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
      private:
        static unsigned int indent_;

        std::string  sig_;
        std::string  file_;
        int          line_;
        std::string  msg_;

        std::string indent (void)
        {
          std::string ret;

          if ( enabled )
          {
            for ( unsigned int i = 0; i < indent_; i++ )
            {
              ret += "  ";
            }
          }

          return ret;
        }


      public:
        static bool enabled;

        stack_tracer (const char * sig, 
                      const char * file, 
                      int          line, 
                      const char * msg = "")
          : sig_      (sig)
          , file_     (file)
          , line_     (line)
          , msg_      (msg)
        {
          std::string out;

          indent_++;

          if ( enabled )
          {
            out += "  --> ";
            out += indent ();

            if ( 0 == sizeof (msg_) )
            {
              out += msg_;
              out += "    > \n";
              out += indent ();
            }

            out += sig_;
            out += " : ";
            out += file_;
            out += " +";
            out += saga::util::itoa (line_);
            out += " -->";

            saga::util::log (saga::util::logging::Debug, "stack trace level" + saga::util::itoa (indent_), out);
          }
        }

        ~stack_tracer (void)
        {
          if ( enabled )
          {
            std::string out;

            out += " <--  ";
            out += indent ();
            out += sig_;
            out += " : ";
            out += file_;
            out += " +";
            out += saga::util::itoa (line_);
            out += " <--";

            saga::util::log (saga::util::logging::Debug, "stack trace level" + saga::util::itoa (indent_), out);
          }

          indent_--;
        }
    };

    unsigned int stack_tracer::indent_ = 0;
    bool         stack_tracer::enabled = false;

// //-------------------------------------------------------------
// #define SAGA_UTIL_TYPEID(x) saga::util::type_id::fn (x, #x)   \
// //-------------------------------------------------------------

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

