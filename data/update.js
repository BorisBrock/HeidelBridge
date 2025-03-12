document.addEventListener("DOMContentLoaded", async function () {
    try {
        // Call the API on the same server
        const response = await fetch("/api/version");

        // Ensure the request was successful
        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        // Parse JSON response
        const data = await response.json();

        // Update the HTML
        document.getElementById("version").textContent = `Currently installed: ${data["version"]} (${data["build_date"]})`;
    } catch (error) {
        console.error(`Error: ${error.message}`);
    }
});

// Shows a nessage box
function messageBox(title, text) {
    document.getElementById("message-box-title").textContent = title;
    document.getElementById("message-box-text").textContent = text;
    toggleModal("message-box");
}

// OTA update
document.getElementById("otaForm").addEventListener("submit", async function (event) {
    event.preventDefault();

    let fileInput = document.getElementById("firmwareFile");
    if (fileInput.files.length === 0) {
        messageBox("Error", "Please select a firmware file!");
        return;
    }

    let formData = new FormData();
    formData.append("firmware", fileInput.files[0]);

    openModal("busy-box");

    try {
        let response = await fetch("/api/update", {
            method: "POST",
            body: formData
        });

        if (response.ok) {
            closeModal("busy-box");
            messageBox("Success", "The firmware was updated successfully. The device will now reboot.");
        } else {
            closeModal("busy-box");
            messageBox("Error", "The firmware update failed.");
        }
    } catch (error) {
        closeModal("busy-box");
        messageBox("Error", error.message);
    }
});