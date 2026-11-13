#pragma once

#include <cstdint>
#include "../Wallbox/IWallbox.h"

namespace DaheimladenRegisterMap
{
    // Modbus limit for a single read holding registers request
    constexpr uint16_t MaxReadWords = 125;

    enum class ReadResult
    {
        Ok,
        UnmappedAddress,  // a register in the range is not mapped (answer with ILLEGAL_DATA_ADDRESS)
        ValueUnavailable, // a value in the range has nothing to report yet (answer with SERVER_DEVICE_BUSY)
    };

    // Fills out[0..count) with the Daheimladen holding registers start..start+count-1.
    // A read may span several values and start or end inside one.
    ReadResult Read(IWallbox &wallbox, const char *stationId, uint16_t start, uint16_t count, uint16_t *out);
};
