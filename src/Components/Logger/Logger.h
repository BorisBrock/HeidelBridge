#pragma once

namespace Logger
{
    // Logs a message with the specified level, format and arguments
    template <typename... Args>
    void Log(const char *level, const char *format, Args... args)
    {
        Serial.print(level);
        Serial.printf(format, args...);
        Serial.print("\n");
    }

    // Logs an error message with the specified format and arguments
    template <typename... Args>
    void Error(const char *format, Args... args)
    {
#if defined(LOGGING_LEVEL_ERROR) || defined(LOGGING_LEVEL_WARNING) || defined(LOGGING_LEVEL_INFO) || defined(LOGGING_LEVEL_DEBUG) || defined(LOGGING_LEVEL_TRACE)
        Log("[ERROR] ", format, args...);
#endif
    }

    // Logs a warning message with the specified format and arguments
    template <typename... Args>
    void Warning(const char *format, Args... args)
    {
#if defined(LOGGING_LEVEL_WARNING) || defined(LOGGING_LEVEL_INFO) || defined(LOGGING_LEVEL_DEBUG) || defined(LOGGING_LEVEL_TRACE)
        Log("[WARNING] ", format, args...);
#endif
    }

    // Logs an informational message with the specified format and arguments
    template <typename... Args>
    void Info(const char *format, Args... args)
    {
#if defined(LOGGING_LEVEL_INFO) || defined(LOGGING_LEVEL_DEBUG) || defined(LOGGING_LEVEL_TRACE)
        Log("[INFO] ", format, args...);
#endif
    }

    // Logs a debug message with the specified format and arguments
    template <typename... Args>
    void Debug(const char *format, Args... args)
    {
#if defined(LOGGING_LEVEL_DEBUG) || defined(LOGGING_LEVEL_TRACE)
        Log("[DEBUG] ", format, args...);
#endif
    }

    // Logs a trace message with the specified format and arguments
    template <typename... Args>
    void Trace(const char *format, Args... args)
    {
#if defined(LOGGING_LEVEL_TRACE)
        Log("[TRACE] ", format, args...);
#endif
    }

    // Prints a message with the specified format and arguments
    template <typename... Args>
    void Print(const char *format, Args... args)
    {
        Serial.printf(format, args...);
        Serial.print("\n");
    }

    // Returns the current logging level as a string
    const char *GetLogLevel();
};