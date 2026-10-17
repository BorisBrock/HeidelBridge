#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <strings.h>
#include "MqttCommandParser.h"

namespace MqttCommandParser
{
    namespace
    {
        // Longest accepted payload after trimming, e.g. "16.000000"
        constexpr size_t MaxValueLength = 16;

        // Strips surrounding whitespace and copies the rest into a null-terminated buffer
        bool Trim(const char *payload, size_t length, char *buffer, size_t bufferSize)
        {
            if (payload == nullptr)
            {
                return false;
            }

            size_t start = 0;
            while (start < length && std::isspace(static_cast<unsigned char>(payload[start])))
            {
                start++;
            }
            size_t end = length;
            while (end > start && std::isspace(static_cast<unsigned char>(payload[end - 1])))
            {
                end--;
            }

            const size_t trimmedLength = end - start;
            if (trimmedLength == 0 || trimmedLength >= bufferSize)
            {
                return false;
            }

            memcpy(buffer, payload + start, trimmedLength);
            buffer[trimmedLength] = '\0';
            return true;
        }
    }

    bool ParseChargingCurrent(const char *payload, size_t length, float &currentA)
    {
        char buffer[MaxValueLength + 1];
        if (!Trim(payload, length, buffer, sizeof(buffer)))
        {
            return false;
        }

        // Only plain decimal numbers: this rules out signs, "nan", "inf", hex and exponents
        bool hasDigit = false;
        bool hasDecimalPoint = false;
        for (const char *c = buffer; *c != '\0'; c++)
        {
            if (std::isdigit(static_cast<unsigned char>(*c)))
            {
                hasDigit = true;
            }
            else if (*c == '.' && !hasDecimalPoint)
            {
                hasDecimalPoint = true;
            }
            else
            {
                return false;
            }
        }
        if (!hasDigit)
        {
            return false;
        }

        char *parseEnd = nullptr;
        const float value = strtof(buffer, &parseEnd);
        if (parseEnd == buffer || *parseEnd != '\0' || !std::isfinite(value))
        {
            return false;
        }

        currentA = value;
        return true;
    }

    bool ParseStandby(const char *payload, size_t length, bool &standbyEnabled)
    {
        char buffer[MaxValueLength + 1];
        if (!Trim(payload, length, buffer, sizeof(buffer)))
        {
            return false;
        }

        if (strcasecmp(buffer, "ON") == 0)
        {
            standbyEnabled = true;
            return true;
        }
        if (strcasecmp(buffer, "OFF") == 0)
        {
            standbyEnabled = false;
            return true;
        }
        return false;
    }
}
