// In-memory replacement for EnergyCounterStorage, which persists to NVS through Settings on the device.
// The native environment excludes the real implementation (build_src_filter).

#include "Components/Wallbox/EnergyCounterStorage.h"

namespace
{
    bool gStored = false;
    int64_t gOffsetWh = 0;
    uint32_t gPublishedWh = 0;
}

bool EnergyCounterStorage::Load(int64_t &offsetWh, uint32_t &publishedWh)
{
    if (!gStored)
    {
        return false;
    }
    offsetWh = gOffsetWh;
    publishedWh = gPublishedWh;
    return true;
}

bool EnergyCounterStorage::SaveOffset(int64_t offsetWh)
{
    gOffsetWh = offsetWh;
    gStored = true;
    return true;
}

bool EnergyCounterStorage::SavePublished(uint32_t publishedWh)
{
    gPublishedWh = publishedWh;
    gStored = true;
    return true;
}
