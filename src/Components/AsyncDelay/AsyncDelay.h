#pragma once

class AsyncDelay
{
public:
    /** Creates an AsyncDelay instance with the given delay in milliseconds
    * @param delayMs The delay duration in milliseconds
    */
    AsyncDelay(uint32_t delayMs);

    // Restarts the delay by setting the start time to the current time.
    // This effectively resets the delay timer.
    void Restart();
    
    /** Checks if the delay has elapsed
    * @return true if the delay duration has passed since the last restart, false otherwise
    */
    bool IsElapsed();

private:
    uint32_t mDelayMs{};
    uint32_t mStartTimeMs{};
};