#pragma once

// Minimal Arduino API mock for host-based unit tests.

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

class SerialClass
{
public:
    void begin(unsigned long baudRate)
    {
    }

    size_t print(const char *text)
    {
        if (text == nullptr)
        {
            return 0;
        }
        std::fputs(text, stdout);
        return std::strlen(text);
    }

    size_t print(char character)
    {
        std::fputc(character, stdout);
        return 1;
    }

    size_t print(int value)
    {
        return static_cast<size_t>(std::fprintf(stdout, "%d", value));
    }

    size_t write(uint8_t byte)
    {
        std::fputc(byte, stdout);
        return 1;
    }

    int printf(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        int result = std::vfprintf(stdout, format, args);
        va_end(args);
        return result;
    }
};

extern SerialClass Serial;

inline void delay(unsigned long milliseconds)
{
}

inline unsigned long millis()
{
    static unsigned long ticks = 0;
    return ++ticks;
}
