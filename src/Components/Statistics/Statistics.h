#pragma once

struct Statistics
{
    uint32_t NumModbusReadErrors = 0;
    uint32_t NumModbusWriteErrors = 0;
    uint32_t NumWifiDisconnects = 0;
    uint32_t NumMqttDisconnects = 0;
    uint64_t UptimeS = 0;
};

// Global statistics instance
extern Statistics gStatistics;