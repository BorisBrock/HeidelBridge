#pragma once

#include <cstdint>
#include "../Wallbox/IWallbox.h"

namespace DaheimladenRegisterMap
{
    // Modbus limit for a single read holding registers request
    constexpr uint16_t MaxReadWords = 125;

    // Fills out[0..count) with the Daheimladen holding registers start..start+count-1.
    // A read may span several values and start or end inside one. Returns false if any
    // register in the range is not mapped (the caller answers with ILLEGAL_DATA_ADDRESS).
    bool Read(IWallbox &wallbox, const char *stationId, uint16_t start, uint16_t count, uint16_t *out);
};
