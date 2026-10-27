const connectButton = document.getElementById("connect-button");
const connectionStatus = document.getElementById("connection-status");
const serialMessage = document.getElementById("serial-message");

let port = null;
let reader = null;
let readTask = null;

async function readMessages(openPort) {
    const decoder = new TextDecoder();
    let buffer = "";

    const activeReader = openPort.readable.getReader();
    reader = activeReader;

    try {
        while (true) {
            const { value, done } = await activeReader.read();

            if (done) {
                break;
            }

            buffer += decoder.decode(value, { stream: true });

            let newlineIndex = buffer.indexOf("\n");

            while (newlineIndex !== -1) {
                const message = buffer.slice(0, newlineIndex).trim();
                buffer = buffer.slice(newlineIndex + 1);

                if (message !== "") {
                    serialMessage.textContent = `Received: ${message}`;
                }

                newlineIndex = buffer.indexOf("\n");
            }

            if (buffer.length > 1024) {
                throw new Error("Received message is too long.");
            }
        }
    } catch (error) {
        connectionStatus.textContent = "Serial reading stopped";
        console.error(error);
    } finally {
        activeReader.releaseLock();
        reader = null;
    }
}

async function selectPort() {
    connectButton.disabled = true;

    try {
        if (port !== null) {
            if (reader !== null) {
                await reader.cancel();
            }

            await readTask;
            await port.close();

            port = null;
            readTask = null;

            connectionStatus.textContent = "Not connected";
            connectButton.textContent = "Connect micro:bit";
        } else {
            const selectedPort = await navigator.serial.requestPort();

            await selectedPort.open({ baudRate: 115200 });
            port = selectedPort;

            connectionStatus.textContent = "Serial port open";
            connectButton.textContent = "Disconnect";
            serialMessage.textContent = "Waiting for a message…";

            readTask = readMessages(port);
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