
#ifndef SAGA_UTIL_STACKTRACER_HPP
#define SAGA_UTIL_STACKTRACER_HPP

#include <iostream>

namespace saga
{
  namespace util
  {
    static unsigned int saga_util_stack_tracer_indent_ = 0;

    class stack_tracer
    {
      private:
        void indent (void)
        {
          for ( unsigned int i = 0; i < saga_util_stack_tracer_indent_; i++ )
          {
            std::cout << "  ";
          }
        }

        std::string func_;
        std::string scope_;
        std::string sig_;
        std::string file_;
        int         line_;
        std::string msg_;


      public:
        stack_tracer (std::string func, 
                      std::string scope, 
                      std::string sig, 
                      std::string file, 
                      int         line, 
                      std::string msg = "")
          : func_     (func)
          , scope_    (scope)
          , sig_      (sig)
          , file_     (file)
          , line_     (line)
          , msg_      (msg)
        {
          saga_util_stack_tracer_indent_++;

          std::cout << "  --> ";
          indent ();

          if ( ! msg_.empty () )
          {
            std::cout << msg_ << std::endl;
            std::cout << "    > ";
            indent ();
          }

          std::cout << scope_ << "::" << func_ << " [" << sig_ << "] in " << file_ << ":" << line_ << std::endl;
        }

        ~stack_tracer (void)
        {
          std::cout << " <--  ";
          indent ();
          std::cout << scope_ << "::" << func_ << " [" << sig_ << "]" << std::endl;

          saga_util_stack_tracer_indent_--;
        }
    };

//-------------------------------------------------------------
#define SAGA_UTIL_STACKTRACE_C(FUNC)                          \
    std::string saga_util_stack_tracer_scope_ ("");           \
    std::string saga_util_stack_tracer_sig_   ("");           \
    try {                                                     \
      saga_util_stack_tracer_sig_   = typeid (FUNC).name ();  \
    } catch (...) {}                                          \
    saga::util::stack_tracer                                  \
    saga_util_stack_tracer_instance_ (                        \
      #FUNC,                                                  \
      saga::util::demangle (saga_util_stack_tracer_scope_),   \
      saga::util::demangle (saga_util_stack_tracer_sig_),     \
      __FILE__, __LINE__)                                     \
//-------------------------------------------------------------

//-------------------------------------------------------------
#define SAGA_UTIL_STACKTRACE(FUNC)                            \
    std::string saga_util_stack_tracer_scope_ ("");           \
    std::string saga_util_stack_tracer_sig_   ("");           \
    try {                                                     \
      saga_util_stack_tracer_scope_ = typeid(*this).name ();  \
      saga_util_stack_tracer_sig_   = typeid (FUNC).name ();  \
    } catch (...) {}                                          \
    saga::util::stack_tracer                                  \
    saga_util_stack_tracer_instance_ (                        \
      #FUNC,                                                  \
      saga::util::demangle (saga_util_stack_tracer_scope_),   \
      saga::util::demangle (saga_util_stack_tracer_sig_),     \
      __FILE__, __LINE__)                                     \
//-------------------------------------------------------------

//-------------------------------------------------------------
#define SAGA_UTIL_STACKTRACE_MSG(FUNC,MSG)                    \
    std::string saga_util_stack_tracer_scope_ ("");           \
    std::string saga_util_stack_tracer_sig_   ("");           \
    try {                                                     \
      saga_util_stack_tracer_scope_ = typeid(*this).name ();  \
      saga_util_stack_tracer_sig_   = typeid (FUNC).name ();  \
    } catch (...) {}                                          \
    saga::util::stack_tracer                                  \
    saga_util_stack_tracer_instance_ (                        \
      #FUNC,                                                  \
      saga::util::demangle (saga_util_stack_tracer_scope_),   \
      saga::util::demangle (saga_util_stack_tracer_sig_),     \
      __FILE__, __LINE__, MSG)                                \
//-------------------------------------------------------------


  } // namespace util

} // namespace saga


#endif // SAGA_UTIL_STACKTRACER_HPP

