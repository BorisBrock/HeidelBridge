#pragma once

namespace Logger
{
    template <typename... Args>
    void Error(const char *format, Args... args)
    {
#if defined(LOGGING_LEVEL_ERROR) || defined(LOGGING_LEVEL_WARNING) || defined(LOGGING_LEVEL_INFO) || defined(LOGGING_LEVEL_DEBUG) || defined(LOGGING_LEVEL_TRACE)
        Serial.print("[ERROR] ");
        Serial.printf(format, args...);
        Serial.print("\n");
#endif
    }

    template <typename... Args>
    void Warning(const char *format, Args... args)
    {
#if defined(LOGGING_LEVEL_WARNING) || defined(LOGGING_LEVEL_INFO) || defined(LOGGING_LEVEL_DEBUG) || defined(LOGGING_LEVEL_TRACE)
        Serial.print("[WARNING] ");
        Serial.printf(format, args...);
        Serial.print("\n");
#endif
    }

    template <typename... Args>
    void Info(const char *format, Args... args)
    {
#if defined(LOGGING_LEVEL_INFO) || defined(LOGGING_LEVEL_DEBUG) || defined(LOGGING_LEVEL_TRACE)
        Serial.print("[INFO] ");
        Serial.printf(format, args...);
        Serial.print("\n");
#endif
    }

    template <typename... Args>
    void Debug(const char *format, Args... args)
    {
#if defined(LOGGING_LEVEL_DEBUG) || defined(LOGGING_LEVEL_TRACE)
        Serial.print("[DEBUG] ");
        Serial.printf(format, args...);
        Serial.print("\n");
#endif
    }

    template <typename... Args>
    void Trace(const char *format, Args... args)
    {
#if defined(LOGGING_LEVEL_TRACE)
        Serial.print("[TRACE] ");
        Serial.printf(format, args...);
        Serial.print("\n");
#endif
    }

    template <typename... Args>
    void Print(const char *format, Args... args)
    {
        Serial.printf(format, args...);
        Serial.print("\n");
    }

    const char *GetLogLevel();
};