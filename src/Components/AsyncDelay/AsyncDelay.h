#pragma once

class AsyncDelay
{
public:
    // Creates an AsyncDelay instance with the given delay in milliseconds
    AsyncDelay(uint32_t delayMs);

    // Restarts the delay
    void Restart();

    // Checks if the delay has elapsed
    bool IsElapsed();

private:
    uint32_t mDelayMs{};
    uint32_t mStartTimeMs{};
};