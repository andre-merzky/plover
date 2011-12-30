
#ifndef SAGA_UTIL_LOGGING_HPP
#define SAGA_UTIL_LOGGING_HPP

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <pthread.h>

#include <saga/util/enums.hpp>
#include <saga/util/mutex.hpp>

#define LOG       saga::util::log
#define LOGSTR    saga::util::logstr

#define NOISE     saga::util::logging::Noise    
#define DEBUG     saga::util::logging::Debug    
#define INFO      saga::util::logging::Info     
#define NOTICE    saga::util::logging::Notice   
#define WARNING   saga::util::logging::Warning  
#define ERROR     saga::util::logging::Error    
#define CRITICAL  saga::util::logging::Critical 
#define ALERT     saga::util::logging::Alert    
#define EMERGENCY saga::util::logging::Emergency


namespace saga
{
  namespace util
  {
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
    // The tag '*' will prompt *all* messages to show, disregarding the tag
    // filters.
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
    // TODO:
    //   - we should implement our own log ostream class, which applies some
    //     formatting (new header for each line, etc)
    //   - flexible line formatting (%i=log index, %s=severity, %m=msg, %p=pid,
    //     %P=tid, %t=tags of message, %T=matched tags)
    //
    class logging
    {
      public:
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

      private:
        // big fat logging lock
        static saga::util::mutex * mtx_;

        unsigned long long                   serial_;

        std::map <pthread_t, std::ostream * >  
                                             streams_;     // map of ostreams per thread
        std::ostream                      *  nostream_;    // logging to /dev/null
        std::string                          spec_;        // output file name spec

        severity                             severity_;

        bool                                 per_thread_;  // one log file per thread
        bool                                 per_process_; // one log file per process

        bool                                 tags_star_;   // '*' is part of tag list
        std::vector <std::string>            tags_may_;
        std::vector <std::string>            tags_must_;
        std::vector <std::string>            tags_must_not_;

        bool matches_severity (severity      s);
        bool matches_tags     (std::string   t);

      public:
        logging  (void);
        ~logging (void);

        // get an opened log stream for this thread/process
        std::ostream & logstr (severity      s,   // severity level of log
                               std::string   t);  // tags for log

        void           log    (severity      s,   // severity level of log
                               std::string   t,   // tags for log
                               std::string   m);  // log message
    };

    SAGA_UTIL_REGISTER_ENUM_S (saga::util::logging::severity, saga__util__logging__severity, Noise    , 0);
    SAGA_UTIL_REGISTER_ENUM_S (saga::util::logging::severity, saga__util__logging__severity, Debug    , 1);
    SAGA_UTIL_REGISTER_ENUM_S (saga::util::logging::severity, saga__util__logging__severity, Info     , 2);
    SAGA_UTIL_REGISTER_ENUM_S (saga::util::logging::severity, saga__util__logging__severity, Notice   , 3);
    SAGA_UTIL_REGISTER_ENUM_S (saga::util::logging::severity, saga__util__logging__severity, Warning  , 4);
    SAGA_UTIL_REGISTER_ENUM_S (saga::util::logging::severity, saga__util__logging__severity, Error    , 5);
    SAGA_UTIL_REGISTER_ENUM_S (saga::util::logging::severity, saga__util__logging__severity, Critical , 6);
    SAGA_UTIL_REGISTER_ENUM_S (saga::util::logging::severity, saga__util__logging__severity, Alert    , 7);
    SAGA_UTIL_REGISTER_ENUM_S (saga::util::logging::severity, saga__util__logging__severity, Emergency, 8);

    typedef saga::util::singleton <saga::util::logging> the_logger;

    // fortcut for logging to the_logger
    void log (saga::util::logging::severity s,   // severity level of message
              std::string                   t,   // tags for message
              std::string                   m);

    // shortcut for getting a log stream from the_loger
    std::ostream & logstr (saga::util::logging::severity s,   // severity level of message
                           std::string                   t);  // tags for message

  } // namespace util

} // namespace saga

#endif // SAGA_UTIL_LOGGING_HPP

