#pragma once

class LockController
{
private:
    // Private constructor to prevent instantiation
    LockController() {};

public:
    // Returns the singleton instance of LockController
    static LockController *Instance();

    // Initializes the LockController instance
    void Init();

    // Sets the lock state (true = unlocked, false = locked)
    void SetLockState(bool unlocked);

    // Gets the current lock state (true = unlocked, false = locked)
    bool GetLockState();

private:
    bool mIsUnlocked = false; // Current lock state
#ifdef ENABLE_LOCK_RELAY
    uint8_t mLockRelayPin = 0; // Pin for controlling the lock relay
#endif
};
