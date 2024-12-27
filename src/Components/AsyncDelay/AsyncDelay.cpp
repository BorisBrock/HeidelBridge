#include <Arduino.h>
#include "AsyncDelay.h"

// AsyncDelay is a class that provides a non-blocking delay mechanism.
// It allows you to set a delay in milliseconds and check if the delay has elapsed without blocking the main loop.
// This is useful for timing events in an asynchronous manner.
AsyncDelay::AsyncDelay(uint32_t delayMs)
    : mDelayMs(delayMs)
{
}

// Restarts the delay by resetting the start time to the current time
void AsyncDelay::Restart()
{
    mStartTimeMs = millis();
}

// Checks if the specified delay has passed since the last restart
bool AsyncDelay::IsElapsed()
{
    uint32_t currentMillis = millis();
    if ((currentMillis - mStartTimeMs) >= mDelayMs)
    {
        return true;
    }

    return false;
}