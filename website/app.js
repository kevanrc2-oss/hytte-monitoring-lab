const connectButton = document.getElementById("connect-button");
const connectionStatus = document.getElementById("connection-status");
const serialMessage = document.getElementById("serial-message");
const homeButton = document.getElementById("mode-home");
const awayButton = document.getElementById("mode-away");
const incidentId = document.getElementById("incident-id");
const incidentSeverity = document.getElementById("incident-severity");
const incidentStatus = document.getElementById("incident-status");
const notResolvedButton = document.getElementById("incident-not-resolved");
const incidentResponse = document.getElementById("incident-response");
const guidanceChoice = document.getElementById("guidance-choice");
const incidentGuidance = document.getElementById("incident-guidance");
const guidanceYes = document.getElementById("guidance-yes");
const guidanceNo = document.getElementById("guidance-no");
const doorOnButton = document.getElementById("door-monitoring-on");
const doorOffButton = document.getElementById("door-monitoring-off");
const doorMonitoringStatus = document.getElementById("door-monitoring-status");
const doorScenarioButton = document.getElementById("scenario-door");
const smokeScenarioButton = document.getElementById("scenario-smoke");
const scenarioStatus = document.getElementById("scenario-status");
const incidentSensor = document.getElementById("incident-sensor");

let port = null;
let reader = null;
let readTask = null;
let writeTask = Promise.resolve();

function updateIncident(message) {
    const unresolved = message.match(
        /^NOT_RESOLVED ([1-9]\d*)$/
    );

    if (unresolved !== null) {
        if (
            unresolved[1] === incidentId.textContent &&
            incidentStatus.textContent === "Active"
        ) {
            incidentResponse.textContent =
                `Not Resolved confirmed. Incident ${unresolved[1]} remains Active.`;

            guidanceChoice.hidden = false;
            incidentGuidance.hidden = true;
        }

        return;
    }

    const match = message.match(
        /^INCIDENT ([1-9]\d*) (DOOR|SMOKE) (Warning|Critical) (Active|Closed)$/
    );

    if (match === null) {
        return;
    }

    if (
        match[1] !== incidentId.textContent ||
        match[4] === "Closed"
    ) {
        incidentResponse.textContent = "";
        guidanceChoice.hidden = true;
        incidentGuidance.hidden = true;
    }

    incidentId.textContent = match[1];
    incidentSensor.textContent = match[2];
    incidentSeverity.textContent = match[3];
    incidentStatus.textContent = match[4];

    notResolvedButton.disabled =
        port === null ||
        reader === null ||
        match[4] !== "Active";
}

function updateDoorMonitoring(message) {
    if (
        message !== "DOOR_MONITORING ON" &&
        message !== "DOOR_MONITORING OFF"
    ) {
        return;
    }

    const enabled = message === "DOOR_MONITORING ON";

    doorMonitoringStatus.textContent =
        enabled ? "Door monitoring: ON" : "Door monitoring: OFF";

    doorOnButton.setAttribute("aria-pressed", String(enabled));
    doorOffButton.setAttribute("aria-pressed", String(!enabled));
}

function updateScenario(message) {
    const match = message.match(/^SCENARIO (DOOR|SMOKE)$/);

    if (match === null) {
        return;
    }

    const isSmoke = match[1] === "SMOKE";

    scenarioStatus.textContent = isSmoke
        ? "Selected scenario: Critical smoke"
        : "Selected scenario: Door activity";

    doorScenarioButton.setAttribute("aria-pressed", String(!isSmoke));
    smokeScenarioButton.setAttribute("aria-pressed", String(isSmoke));
}

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
                    updateIncident(message);
                    updateDoorMonitoring(message);
                    updateScenario(message);
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

        notResolvedButton.disabled = true;
        doorOnButton.disabled = true;
        doorOffButton.disabled = true;
        doorScenarioButton.disabled = true;
        smokeScenarioButton.disabled = true;
        homeButton.disabled = true;
        awayButton.disabled = true;

        guidanceChoice.hidden = true;
        incidentGuidance.hidden = true;
        
    }
}

function sendCommand(command) {
    const selectedPort = port;

    writeTask = writeTask.catch(() => {}).then(async () => {
        if (selectedPort === null || selectedPort !== port) {
            throw new Error("No active serial connection.");
        }

        const writer = selectedPort.writable.getWriter();

        try {
            const data = new TextEncoder().encode(command + "\n");

            await writer.write(data);
        } finally {
            writer.releaseLock();
        }
    });

    return writeTask;
}

async function selectPort() {
    connectButton.disabled = true;

    try {
        if (port !== null) {
            homeButton.disabled = true;
            awayButton.disabled = true;
            notResolvedButton.disabled = true;
            doorOnButton.disabled = true;
            doorOffButton.disabled = true;

            doorScenarioButton.disabled = true;
            smokeScenarioButton.disabled = true;

            if (reader !== null) {
                await reader.cancel();
            }

            await readTask;
            await writeTask.catch(() => {});
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
            incidentId.textContent = "—";
            incidentSensor.textContent = "—";
            incidentSeverity.textContent = "—";
            incidentStatus.textContent = "—";
            incidentResponse.textContent = "";
            notResolvedButton.disabled = true;
            readTask = readMessages(port);
            homeButton.disabled = false;
            awayButton.disabled = false;
            doorOnButton.disabled = false;
            doorOffButton.disabled = false;
           // Aktiver valg av scenario etter tilkobling.
            doorScenarioButton.disabled = false;
            smokeScenarioButton.disabled = false;

            scenarioStatus.textContent = "Selected scenario: not confirmed";
            doorScenarioButton.setAttribute("aria-pressed", "false");
            smokeScenarioButton.setAttribute("aria-pressed", "false");

            doorMonitoringStatus.textContent = "Door monitoring: not confirmed";  
            doorOnButton.setAttribute("aria-pressed", "false");
            doorOffButton.setAttribute("aria-pressed", "false");
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

homeButton.addEventListener("click", () => {
    sendCommand("MODE HOME").catch(error => {
        serialMessage.textContent = "Could not send HOME command";
        console.error(error);
    });
});

awayButton.addEventListener("click", () => {
    sendCommand("MODE AWAY").catch(error => {
        serialMessage.textContent = "Could not send AWAY command";
        console.error(error);
    });
});

notResolvedButton.addEventListener("click", () => {
    const id = incidentId.textContent;

    notResolvedButton.disabled = true;

    sendCommand(`NOT_RESOLVED ${id}`)
        .catch(error => {
            incidentResponse.textContent =
                "Could not send Not Resolved response.";

            console.error(error);
        })
        .finally(() => {
            notResolvedButton.disabled =
                port === null ||
                reader === null ||
                incidentStatus.textContent !== "Active";
        });
});

guidanceYes.addEventListener("click", () => {
    guidanceChoice.hidden = true;
    incidentGuidance.hidden = false;
});

guidanceNo.addEventListener("click", () => {
    guidanceChoice.hidden = true;
    incidentGuidance.hidden = true;

    incidentResponse.textContent =
        "Guidance declined. The incident remains Active.";
});

doorOnButton.addEventListener("click", () => {
    sendCommand("DOOR ON").catch(error => {
        serialMessage.textContent = "Could not enable Door monitoring.";
        console.error(error);
    });
});

doorOffButton.addEventListener("click", () => {
    sendCommand("DOOR OFF").catch(error => {
        serialMessage.textContent = "Could not disable Door monitoring.";
        console.error(error);
    });
});

function selectScenario(scenario) {
    sendCommand(`SELECT ${scenario}`).catch(error => {
        serialMessage.textContent = "Could not select scenario.";
        console.error(error);
    });
}

doorScenarioButton.addEventListener("click", () => {
    selectScenario("DOOR");
});

smokeScenarioButton.addEventListener("click", () => {
    selectScenario("SMOKE");
});