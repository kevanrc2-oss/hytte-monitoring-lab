#include "MicroBit.h"
#include <string>

MicroBit uBit;

enum class MonitoringMode
{
    Unselected,
    Home,
    Away
};

MonitoringMode currentMode = MonitoringMode::Unselected;

void reportMode()
{
    switch (currentMode)
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

void onButtonA(MicroBitEvent)
{
    uBit.serial.send(ManagedString("BUTTON_A\n"), SYNC_SLEEP);
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
            currentMode = MonitoringMode::Home;
            reportMode();
        }
        else if (command == "MODE AWAY")
        {
            currentMode = MonitoringMode::Away;
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

    reportMode();

    create_fiber(receiveCommands);

    release_fiber();
}