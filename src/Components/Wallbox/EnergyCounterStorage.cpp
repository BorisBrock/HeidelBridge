#include <Arduino.h>
#include "../Logger/Logger.h"
#include "../../Configuration/Settings.h"
#include "EnergyCounterStorage.h"

bool EnergyCounterStorage::Load(int64_t &offsetWh, uint32_t &publishedWh)
{
    return Settings::Instance()->ReadEnergyCounter(offsetWh, publishedWh);
}

bool EnergyCounterStorage::SaveOffset(int64_t offsetWh)
{
    if (!Settings::Instance()->WriteEnergyOffset(offsetWh))
    {
        Logger::Error("Energy counter: ERROR: Could not persist offset");
        return false;
    }
    return true;
}

bool EnergyCounterStorage::SavePublished(uint32_t publishedWh)
{
    if (!Settings::Instance()->WriteEnergyPublished(publishedWh))
    {
        Logger::Error("Energy counter: ERROR: Could not persist counter value");
        return false;
    }
    return true;
}
