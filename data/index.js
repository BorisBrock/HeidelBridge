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
        document.getElementById("label_version").textContent = `Current firmware version is ${data["version"]}`;
    } catch (error) {
        console.error(`Error: ${error.message}`);
    }
});

function updateFirmware() {
    // Show file browser
    document.getElementById('fileInput').addEventListener('change', function (event) {
        const file = event.target.files[0];
        const info = `File Name: ${file.name} <br> File Size: ${file.size} bytes<br> File Type: ${file.type}`;
        //document.getElementById('fileInfo').innerHTML = info;
    });
}