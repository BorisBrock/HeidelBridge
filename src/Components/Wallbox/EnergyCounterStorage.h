#pragma once

#include "EnergyCounter.h"

// Stores the energy counter state in the settings NVS namespace
class EnergyCounterStorage : public EnergyCounter::Persistence
{
public:
    virtual bool Load(int64_t &offsetWh, uint32_t &publishedWh) override;
    virtual bool SaveOffset(int64_t offsetWh) override;
    virtual bool SavePublished(uint32_t publishedWh) override;
};
