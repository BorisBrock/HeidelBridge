#pragma once

#include <cstddef>

// Strict parsers for MQTT control payloads. Payloads are not null-terminated.
// Anything that is not clearly valid is rejected, so a malformed message never
// changes the wallbox state.
namespace MqttCommandParser
{
    // Parses a charging current in Amperes, e.g. "10", "6.5" or " 16.0 ".
    // Only digits, one optional decimal point and surrounding whitespace are
    // accepted; negative, non-finite or empty values are rejected.
    bool ParseChargingCurrent(const char *payload, size_t length, float &currentA);

    // Parses a standby command: "ON" or "OFF" (case-insensitive, surrounding
    // whitespace allowed).
    bool ParseStandby(const char *payload, size_t length, bool &standbyEnabled);
}
