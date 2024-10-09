#include <Arduino.h>
#include "AsyncDelay.h"

// Creates an AsyncDelay instance with the given delay in milliseconds
AsyncDelay::AsyncDelay(uint32_t delayMs) : mDelayMs(delayMs)
{
}

// Restarts the delay
void AsyncDelay::Restart()
{
    mStartTimeMs = millis();
}

// Checks if the delay has elapsed
bool AsyncDelay::IsElapsed()
{
    uint32_t currentMillis = millis();
    if (currentMillis - mStartTimeMs >= mDelayMs)
    {
        return true;
    }

    return false;
}