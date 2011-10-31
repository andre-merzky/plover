
#ifndef SAGA_UTIL_STACKTRACER_HPP
#define SAGA_UTIL_STACKTRACER_HPP

#include <iostream>

namespace saga
{
  namespace util
  {
    class stack_tracer
    {
      private:
        static unsigned int indent_;

        const char * sig_;
        const char * file_;
        int          line_;
        const char * msg_;

        void indent (void)
        {
          if ( enabled )
          {
            for ( unsigned int i = 0; i < indent_; i++ )
            {
              std::cout << "    ";
            }
          }
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
          indent_++;

          if ( enabled )
          {
            std::cout << "  --> ";
            indent ();

            if ( 0 == sizeof (msg_) )
            {
              std::cout << msg_ << std::endl;
              std::cout << "    > ";
              indent ();
            }

            std::cout << sig_ << " : " << file_ << " +" << line_ << " -->" << std::endl;
          }
        }

        ~stack_tracer (void)
        {
          if ( enabled )
          {
            std::cout << " <--  ";
            indent ();
            std::cout << sig_ << " : " << file_ << " +" << line_ << " <--" << std::endl;
          }

          indent_--;
        }
    };

    unsigned int stack_tracer::indent_ = 0;
    bool         stack_tracer::enabled = false;

#define SAGA_UTIL_TYPEID(x) saga::util::type_id::fn (x, #x)

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

