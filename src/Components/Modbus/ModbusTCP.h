#pragma once

#include "../Wallbox/IWallbox.h"

namespace ModbusTCP
{
    // Initializes the ModbusTCP server
    void Init(IWallbox *wallbox);
};