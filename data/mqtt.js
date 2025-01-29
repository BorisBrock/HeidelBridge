document.addEventListener("DOMContentLoaded", async function () {
    try {
        // Call the API on the same server
        const response = await fetch("/api/index");

        // Ensure the request was successful
        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        // Parse JSON response
        const data = await response.json();
        
        // Update the HTML
        //document.getElementById("lbl_firmware_state").textContent = `Current firmware version is ${data["version"]}`;
    } catch (error) {
        console.error(`Error: ${error.message}`);
    }
});