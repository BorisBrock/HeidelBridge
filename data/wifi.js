var IsScanRunning = false;

document.addEventListener("DOMContentLoaded", async function () {
    startNetworkScan();
});

async function startNetworkScan() {
    if(IsScanRunning) {
        return;
    }
    IsScanRunning = true;

    // Disable the button
    document.getElementById("button-scan").setAttribute("disabled", "disabled");

    // Clear the list
    document.getElementById("wifi-list").innerHTML = "Scanning for networks...";

    // Send the API request
    await fetch("/api/wifi_scan_start");
    checkScanResults();
}

async function checkScanResults() {
    let response = await fetch('/api/wifi_scan_status');
    let data = await response.json();

    let list = document.getElementById("wifi-list");
    
    if (data.status === "scanning") {
        // Try again in 250 ms
        setTimeout(checkScanResults, 250);
    } else {
        // Enable the button
        document.getElementById("button-scan").removeAttribute("disabled");
        IsScanRunning = false;

        // Update list
        data.networks.forEach(net => {
            //let icon = getSignalIcon(net.rssi);
            list.innerHTML += `<li>${net.ssid} (${net.rssi} dBm)</li>`;
        });
    }
}