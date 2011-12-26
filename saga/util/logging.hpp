
#ifndef SAGA_UTIL_LOGGING_HPP
#define SAGA_UTIL_LOGGING_HPP

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <pthread.h>

#include <saga/util/mutex.hpp>
#include <saga/util/enums.hpp>

namespace saga
{
  namespace util
  {
    enum severity
    {
      Noise     = 0,
      Debug     = 1,
      Info      = 2,
      Notice    = 3,
      Warning   = 4,
      Error     = 5,
      Critical  = 6,
      Alert     = 7,
      Emergency = 8
    };

    SAGA_UTIL_REGISTER_ENUM (severity, Noise    , 0);
    SAGA_UTIL_REGISTER_ENUM (severity, Debug    , 1);
    SAGA_UTIL_REGISTER_ENUM (severity, Info     , 2);
    SAGA_UTIL_REGISTER_ENUM (severity, Notice   , 3);
    SAGA_UTIL_REGISTER_ENUM (severity, Warning  , 4);
    SAGA_UTIL_REGISTER_ENUM (severity, Error    , 5);
    SAGA_UTIL_REGISTER_ENUM (severity, Critical , 6);
    SAGA_UTIL_REGISTER_ENUM (severity, Alert    , 7);
    SAGA_UTIL_REGISTER_ENUM (severity, Emergency, 8);

    //////////////////////////////////////////////////////////////////
    //
    // The logging class allows for thread safe logging, and supports
    // severity classes (aja log levels), tags, and different output 
    // targets (files, stdout).
    //
    // Supported severity levels are
    //
    //   noise            lucy in the sky with diamonds *tralala*
    //   debug            the sky is above.
    //   info             the sky is blue.
    //   notice           the sky is blue today.
    //   warning          the sky looks like rain!
    //   error            the sky is green, oops!
    //   critical         sky shows signs of high winds
    //   alert            tornado alert
    //   emergency        tornado incoming
    //
    // Tags are set for each log message as follows (pseudo code):
    //
    //   logger (serverity, [adaptor, file, stack], "hello world");
    //
    // The message output can be filtered on tags like this
    //
    //   !adaptor,!file   only show if both tags are set
    //   adaptor,core     show if either tag is set
    //   adaptor,^job     show if adaptor is set and job is unset
    //   ^core,file       show if file is set, cor is unset
    // 
    // The output if filtered by the environment variables
    //
    //   SAGA_LOG_SEVERITY
    //   SAGA_LOG_TAGS
    //   SAGA_LOG_TARGET
    //
    // Allowed output targets are
    //
    //   stdout                 standard output
    //   stderr                 standard error
    //   somefile.%t.%p.%d.log  some (set of) output file(s)
    //
    // where the output file specifier can have the following
    // placeholders:
    //
    //   %t     thread id
    //   %p     process id
    //   %d     job startup time (actual logging init time)
    //
    // Note that log files which contain ythread or job ids are truncated on
    // opening - otherwise messages are appended to the log files.
    //
    //
    class logging
    {
      public:
      private:
        // big fat logging lock
        static saga::util::mutex * mtx_;

        std::map <pthread_t, std::ostream * >  
                                             streams_;     // map of ostreams per thread
        std::string                          spec_;        // output file name spec

        saga::util::severity                 severity_;

        bool                                 per_thread_;  // one log file per thread
        bool                                 per_process_; // one log file per process

        std::string                          tags_str_;
        std::vector <std::string>            tags_may_;
        std::vector <std::string>            tags_must_;
        std::vector <std::string>            tags_must_not_;

        // get an opened log target for this thread/process
        std::ostream & str_ (void);

        bool matches_severity (saga::util::severity s);
        bool matches_tags     (std::string          t);

      public:
        logging  (void);
        ~logging (void);

        void log (saga::util::severity s,   // severity level of message
                  std::string          t,   // tags for message
                  std::string          m);  // log message
    };

    typedef saga::util::singleton <saga::util::logging> the_logger;

    // fortcut for logging to the_logger
    void log (saga::util::severity s,   // severity level of message
              std::string          t,   // tags for message
              std::string          m);

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_LOGGING_HPP

