document.addEventListener("DOMContentLoaded", async function () {
    try {
        // Call the API on the same server
        const response = await fetch("/api/settings_read");

        // Ensure the request was successful
        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        // Parse JSON response
        const data = await response.json();

        // Update the HTML
        document.getElementById("device-name").value = data["device-name"];
        document.getElementById("wifi-ssid").value = data["wifi-ssid"];
        document.getElementById("wifi-password").value = data["wifi-password"];
        document.getElementById("mqtt-enabled").checked = data["mqtt-enabled"];
        document.getElementById("mqtt-server").value = data["mqtt-server"];
        document.getElementById("mqtt-port").value = data["mqtt-port"];
        document.getElementById("mqtt-user").value = data["mqtt-user"];
        document.getElementById("mqtt-password").value = data["mqtt-password"];
    } catch (error) {
        console.error(`Error: ${error.message}`);
    }
});

function messageBox(title, text) {
    document.getElementById("message-box-title").textContent = title;
    document.getElementById("message-box-text").textContent = text;
    toggleModal("message-box");
}

function writeSettings() {
    // Write the settings from the UI to the device
    const data = {
        "device-name": document.getElementById("device-name").value,
        "wifi-ssid": document.getElementById("wifi-ssid").value,
        "wifi-password": document.getElementById("wifi-password").value,
        "mqtt-enabled": document.getElementById("mqtt-enabled").checked,
        "mqtt-server": document.getElementById("mqtt-server").value,
        "mqtt-port": parseInt(document.getElementById("mqtt-port").value),
        "mqtt-user": document.getElementById("mqtt-user").value,
        "mqtt-password": document.getElementById("mqtt-password").value
    };
    fetch("/api/settings_write", {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify(data)
    })
        .then(response => {
            if (!response.ok) {
                throw new Error(`HTTP error! Status: ${response.status}`);
            }

            // Show a success message popup
            messageBox("Success", "The device will now reboot to apply your changes.");
            restartEsp();
        })
        .catch(error => {
            console.error(`Error: ${error.message}`);
        });
}

function restartEsp() {
    fetch("/api/reboot", {
        method: "POST"
    })
        .catch(error => {
            console.error(`Error: ${error.message}`);
        });
}
