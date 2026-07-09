document.addEventListener("DOMContentLoaded", () => {
    const colorButton = document.getElementById("colorButton");

    if (colorButton) {
        colorButton.addEventListener("click", () => {
            document.body.style.backgroundColor = "#e8f5e9";

            const status = document.getElementById("status");

            if (status) {
                status.textContent =
                    "JavaScript loaded successfully!";
            }
        });
    }

    const helloButton = document.getElementById("helloButton");

    if (helloButton) {
        helloButton.addEventListener("click", () => {
            const message = document.getElementById("message");

            if (message) {
                message.textContent =
                    "Hello from app.js!";
            }
        });
    }
});