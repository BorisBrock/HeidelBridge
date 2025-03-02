var IsScanRunning = false;

document.addEventListener("DOMContentLoaded", async function () {
    startNetworkScan();
});

async function startNetworkScan() {
    if (IsScanRunning) {
        return;
    }
    IsScanRunning = true;

    // Disable the button
    let button = document.getElementById("button-scan");
    button.setAttribute("disabled", "disabled");
    button.innerHTML = "Scanning for networks...";

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
        let button = document.getElementById("button-scan");
        button.removeAttribute("disabled");
        button.innerHTML = "Scan for networks";

        // Update list
        data.networks.forEach(net => {
            //let icon = getSignalIcon(net.rssi);
            let item = document.createElement("li");
            item.innerHTML = `${net.ssid} <small>(${net.rssi} dBm)</small>`;
            item.onclick = () => selectNetwork(net.ssid);
            list.appendChild(item);
        });
    }
}

async function selectNetwork(ssid) {
    let password = prompt("Password", "");
    if (password != null) {
        fetch('/api/wifi_connect');
    }

    let encodedSSID = encodeURIComponent(ssid);
    let encodedPassword = encodeURIComponent(password);

    let response = await fetch(`/api/wifi_connect?ssid=${encodedSSID}&password=${encodedPassword}`);
    //let result = await response.json();

    alert("Establishing connection. HeidelBridge will now reboot...");
}