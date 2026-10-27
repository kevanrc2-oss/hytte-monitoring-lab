#include "MicroBit.h"
#include <string>
#include "MonitoringSystem.h"
#include <cstdio>

MicroBit uBit;

MonitoringSystem monitoring;

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

void reportDoorIncident(const char* status)
{
    char message[100];

    std::snprintf(
        message,
        sizeof(message),
        "INCIDENT %u DOOR Warning %s\n",
        monitoring.getDoorIncidentId(),
        status
    );

    uBit.serial.send(ManagedString(message), SYNC_SLEEP);
}

void onButtonA(MicroBitEvent)
{
    const DoorResult result = monitoring.detectDoorActivity();

    if (result == DoorResult::ModeNotSelected)
    {
        uBit.serial.send(
            ManagedString("ERROR MODE_NOT_SELECTED\n"),
            SYNC_SLEEP
        );
        return;
    }

    if (result == DoorResult::Ignored)
    {
        uBit.serial.send(
            ManagedString("DOOR IGNORED\n"),
            SYNC_SLEEP
        );
        return;
    }

        uBit.display.printChar('W');
        reportDoorIncident("Active");
}

void onButtonB(MicroBitEvent)
{
    if (!monitoring.resolveDoorIncident())
    {
        uBit.serial.send(
            ManagedString("ERROR NO_ACTIVE_DOOR_INCIDENT\n"),
            SYNC_SLEEP
        );
        return;
    }

    reportMode();
    reportDoorIncident("Closed");
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

    create_fiber(receiveCommands);

    release_fiber();
}