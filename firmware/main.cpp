#include "MicroBit.h"

MicroBit uBit;

void onButtonA(MicroBitEvent)
{
    uBit.display.printChar('A');

    uBit.serial.send(
        ManagedString("BUTTON_A\n"),
        SYNC_SLEEP
    );
}

int main()
{
    uBit.init();

    uBit.serial.setBaud(115200);

    uBit.messageBus.listen(
        MICROBIT_ID_BUTTON_A,
        MICROBIT_BUTTON_EVT_CLICK,
        onButtonA
    );

    uBit.display.printChar('R');

    release_fiber();
}