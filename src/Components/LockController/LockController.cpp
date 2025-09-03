#include <Arduino.h>
#include "../Logger/Logger.h"
#include "../../Boards/BoardFactory.h"
#include "../../Boards/Board.h"
#include "LockController.h"

// Returns the singleton instance of LockController
LockController *LockController::Instance()
{
    static LockController instance;
    return &instance;
}

// Initializes the LockController instance
void LockController::Init()
{
#ifdef ENABLE_LOCK_RELAY
    // Get the lock relay pin from the board configuration
    mLockRelayPin = BoardFactory::Instance()->GetBoard()->GetPinLockRelay();
    
    Logger::Info("Initializing lock controller on pin %d", mLockRelayPin);
    
    // Initialize the pin (already done in Board::Init(), but ensure it's set correctly)
    pinMode(mLockRelayPin, OUTPUT);
    SetLockState(false); // Default to locked state
    
    Logger::Info("Lock controller initialized successfully");
#else
    Logger::Info("Lock controller disabled (ENABLE_LOCK_RELAY not defined)");
#endif
}

// Sets the lock state (true = unlocked, false = locked)
void LockController::SetLockState(bool unlocked)
{
#ifdef ENABLE_LOCK_RELAY
    mIsUnlocked = unlocked;
    
    // Set the relay pin (HIGH = unlocked, LOW = locked)
    digitalWrite(mLockRelayPin, unlocked ? HIGH : LOW);
    
    Logger::Info("Lock state changed to: %s (pin %d set to %s)", 
                unlocked ? "unlocked" : "locked", 
                mLockRelayPin,
                unlocked ? "HIGH" : "LOW");
    
    // Debug: Read back the pin state to verify
    int pinState = digitalRead(mLockRelayPin);
    Logger::Debug("Pin %d readback value: %d", mLockRelayPin, pinState);
#else
    // Update state but don't control any pin
    mIsUnlocked = unlocked;
    Logger::Info("Lock state changed to: %s (no physical relay control)", unlocked ? "unlocked" : "locked");
#endif
}

// Gets the current lock state (true = unlocked, false = locked)
bool LockController::GetLockState()
{
    return mIsUnlocked;
}
