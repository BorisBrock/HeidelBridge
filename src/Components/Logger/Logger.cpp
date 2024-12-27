#include <Arduino.h>
#include "Logger.h"

namespace Logger
{
    const char *GetLogLevel()
    {
#ifdef LOGGING_LEVEL_ERROR
        return "Error";
#elif defined(LOGGING_LEVEL_WARNING)
        return "Warning";
#elif defined(LOGGING_LEVEL_INFO)
        return "Info";
#elif defined(LOGGING_LEVEL_DEBUG)
        return "Debug";
#elif defined(LOGGING_LEVEL_TRACE)
        return "Trace";
#else
        return "Undefined";
#endif
    }
};