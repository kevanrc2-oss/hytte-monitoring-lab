const connectButton = document.getElementById("connect-button");
const connectionStatus = document.getElementById("connection-status");

let port = null;

async function selectPort() {
    connectButton.disabled = true;

    try {
        if (port !== null) {
            await port.close();
            port = null;

            connectionStatus.textContent = "Not connected";
            connectButton.textContent = "Connect micro:bit";
        } else {
            const selectedPort = await navigator.serial.requestPort();

            await selectedPort.open({ baudRate: 115200 });
            port = selectedPort;

            connectionStatus.textContent = "Serial port open";
            connectButton.textContent = "Disconnect";
        }
    } catch (error) {
        if (error.name === "NotFoundError") {
            connectionStatus.textContent = "No port selected";
        } else {
            connectionStatus.textContent = "Connection error";
            console.error(error);
        }
    } finally {
        connectButton.disabled = false;
    }
}

if (window.isSecureContext && "serial" in navigator) {
    connectButton.disabled = false;
    connectButton.addEventListener("click", selectPort);
} else {
    connectionStatus.textContent = "Web Serial unavailable";
}