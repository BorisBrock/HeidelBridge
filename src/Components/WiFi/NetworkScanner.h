#pragma once

namespace NetworkScanner
{
    // Scans for available WiFi networks
    void StartNetworkScan();

    // Checks if the scan has finished
    bool IsNetworkScanRunning();

    // Gets the network scan results
    void GetNetworkScanResults(JsonDocument &jsonDoc);
};