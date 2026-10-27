#include "MicroBit.h"
#include <string>
#include "MonitoringSystem.h"
#include <cstdio>
#include "Sound.h"

MicroBit uBit;

MonitoringSystem monitoring;

enum class DemoScenario
{
    Door,
    Smoke
};

DemoScenario selectedScenario = DemoScenario::Door;

void reportMode()
{
    switch (monitoring.getMode())
    {
        case MonitoringMode::Home:
            uBit.display.printChar('H');
            uBit.serial.send(ManagedString("MODE HOME\n"), SYNC_SLEEP);
            break;

        case MonitoringMode::Away:
            uBit.display.printChar('A');
            uBit.serial.send(ManagedString("MODE AWAY\n"), SYNC_SLEEP);
            break;

        case MonitoringMode::Unselected:
            uBit.display.printChar('?');
            uBit.serial.send(ManagedString("MODE UNSELECTED\n"), SYNC_SLEEP);
            break;
    }
}

void reportIncident(DemoScenario scenario, const char* status)
{
    const bool isSmoke = scenario == DemoScenario::Smoke;

    const char* sensor = isSmoke ? "SMOKE" : "DOOR";
    const char* severity = isSmoke ? "Critical" : "Warning";

    const unsigned int id = isSmoke
        ? monitoring.getSmokeIncidentId()
        : monitoring.getDoorIncidentId();

    char message[100];

    std::snprintf(
        message,
        sizeof(message),
        "INCIDENT %u %s %s %s\n",
        id,
        sensor,
        severity,
        status
    );

    uBit.serial.send(ManagedString(message), SYNC_SLEEP);
}

void onButtonA(MicroBitEvent)
{
    if (monitoring.getMode() == MonitoringMode::Unselected)
    {
        uBit.serial.send(
            ManagedString("ERROR MODE_NOT_SELECTED\n"),
            SYNC_SLEEP
        );
        return;
    }

    const DemoScenario scenario = selectedScenario;
    bool newIncident = false;

    if (scenario == DemoScenario::Door)
    {
        const DoorResult result = monitoring.detectDoorActivity();

        if (result == DoorResult::Ignored)
        {
            uBit.serial.send(
                ManagedString("DOOR IGNORED\n"),
                SYNC_SLEEP
            );
            return;
        }

        newIncident = result == DoorResult::WarningCreated;
        uBit.display.printChar('W');
    }
    else
    {
        const SmokeResult result = monitoring.detectSmokeActivity();

        newIncident = result == SmokeResult::CriticalCreated;
        uBit.display.printChar('C');
    }

    reportIncident(scenario, "Active");

    if (newIncident)
    {
        if (scenario == DemoScenario::Door)
        {
            playWarningSound(uBit);
        }
        else
        {
            playCriticalSound(uBit);
        }
    }
}

void onButtonB(MicroBitEvent)
{
    const DemoScenario scenario = selectedScenario;

    const bool resolved =
        scenario == DemoScenario::Smoke
            ? monitoring.resolveSmokeIncident()
            : monitoring.resolveDoorIncident();

    if (!resolved)
    {
        uBit.serial.send(
            ManagedString("ERROR NO_ACTIVE_INCIDENT\n"),
            SYNC_SLEEP
        );
        return;
    }

    reportMode();
    reportIncident(scenario, "Closed");

    playResolvedSound(uBit);
}

void reportDoorMonitoring()
{
    const char* message = monitoring.isDoorMonitoringEnabled()
        ? "DOOR_MONITORING ON\n"
        : "DOOR_MONITORING OFF\n";

    uBit.serial.send(ManagedString(message), SYNC_SLEEP);
}

void reportSelectedScenario()
{
    const char* message = selectedScenario == DemoScenario::Smoke
        ? "SCENARIO SMOKE\n"
        : "SCENARIO DOOR\n";

    uBit.serial.send(ManagedString(message), SYNC_SLEEP);
}

void receiveCommands()
{
    while (true)
    {
        ManagedString received = uBit.serial.readUntil(
            ManagedString("\n"),
            SYNC_SLEEP
        );

        std::string command(received.toCharArray());

        if (!command.empty() && command.back() == '\r')
        {
            command.pop_back();
        }

        if (command == "MODE HOME")
        {
            monitoring.setMode(MonitoringMode::Home);
            reportMode();
        }
        else if (command == "MODE AWAY")
        {
            monitoring.setMode(MonitoringMode::Away);
            reportMode();
        }
                else if (command == "GET_MODE")
        {
            reportMode();
        }
                else if (command == "SELECT DOOR")
        {
            selectedScenario = DemoScenario::Door;
            reportSelectedScenario();
        }
        else if (command == "SELECT SMOKE")
        {
            selectedScenario = DemoScenario::Smoke;
            reportSelectedScenario();
        }
        else if (command == "GET_SCENARIO")
        {
            reportSelectedScenario();
        }
                else if (command == "DOOR ON")
        {
            monitoring.setDoorMonitoringEnabled(true);
            reportDoorMonitoring();
        }
        else if (command == "DOOR OFF")
        {
            monitoring.setDoorMonitoringEnabled(false);
            reportDoorMonitoring();
        }
        else if (command == "GET_DOOR_MONITORING")
        {
            reportDoorMonitoring();
        }
        else if (command.compare(0, 13, "NOT_RESOLVED ") == 0)
        {
            unsigned int incidentId = 0;
            char extra;

            const int fields = std::sscanf(
                command.c_str(),
                "NOT_RESOLVED %u %c",
                &incidentId,
                &extra
            );

            bool accepted = false;
            DemoScenario incidentScenario = DemoScenario::Door;

            if (fields == 1)
            {
                if (monitoring.markDoorNotResolved(incidentId))
                {
                    accepted = true;
                }
                else if (monitoring.markSmokeNotResolved(incidentId))
                {
                    accepted = true;
                    incidentScenario = DemoScenario::Smoke;
                }
            }

            if (!accepted)
            {
                uBit.serial.send(
                    ManagedString("ERROR INCIDENT_NOT_ACTIVE\n"),
                    SYNC_SLEEP
                );
            }
            else
            {
                selectedScenario = incidentScenario;
                reportSelectedScenario();

                uBit.display.printChar(
                    incidentScenario == DemoScenario::Smoke ? 'C' : 'W'
                );

                reportIncident(incidentScenario, "Active");

                char response[60];

                std::snprintf(
                    response,
                    sizeof(response),
                    "NOT_RESOLVED %u\n",
                    incidentId
                );

                uBit.serial.send(ManagedString(response), SYNC_SLEEP);
            }
        }
        else
        {
            uBit.serial.send(
                ManagedString("ERROR UNKNOWN_COMMAND\n"),
                SYNC_SLEEP
            );
        }
    }
}

int main()
{
    uBit.init();
    uBit.serial.setBaud(115200);
    uBit.serial.setRxBufferSize(128);

    uBit.messageBus.listen(
        MICROBIT_ID_BUTTON_A,
        MICROBIT_BUTTON_EVT_CLICK,
        onButtonA
    );

        uBit.messageBus.listen(
        MICROBIT_ID_BUTTON_B,
        MICROBIT_BUTTON_EVT_CLICK,
        onButtonB
    );

    reportMode();
    reportSelectedScenario();

    create_fiber(receiveCommands);

    release_fiber();
}