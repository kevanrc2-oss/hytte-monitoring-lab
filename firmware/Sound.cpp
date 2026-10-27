#include "Sound.h"

namespace
{
    void playTone(MicroBit& board, int frequencyHz, int durationMs)
    {
        board.audio.setSpeakerEnabled(true);

        board.audio.virtualOutputPin.setAnalogPeriodUs(
            1000000 / frequencyHz
        );

        board.audio.virtualOutputPin.setAnalogValue(64);

        board.sleep(durationMs);

        board.audio.virtualOutputPin.setAnalogValue(0);
    }
}

void playWarningSound(MicroBit& board)
{
    playTone(board, 880, 120);
    board.sleep(100);
    playTone(board, 880, 120);
}

void playCriticalSound(MicroBit& board)
{
    for (int i = 0; i < 3; ++i)
    {
        playTone(board, 1760, 90);
        board.sleep(70);
    }
}

void playResolvedSound(MicroBit& board)
{
    playTone(board, 660, 120);
    playTone(board, 990, 180);
}