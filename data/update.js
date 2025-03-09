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
