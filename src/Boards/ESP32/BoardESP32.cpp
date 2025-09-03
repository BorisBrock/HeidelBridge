#include <Arduino.h>
#include "Components/Logger/Logger.h"
#include "../Board.h"
#include "BoardESP32.h"

// Pin connections:
// ESP32 GPIO18 -> MAXRS485 RO (Receiver Output)
// ESP32 GPIO19 -> MAXRS485 DI (Driver Input)
// ESP32 GPIO21 -> MAXRS485 DE+RE
constexpr uint8_t PinRX = GPIO_NUM_18;
constexpr uint8_t PinTX = GPIO_NUM_19;
constexpr uint8_t PinRTS = GPIO_NUM_21;
#ifdef ENABLE_LOCK_RELAY
constexpr uint8_t PinLockRelay = GPIO_NUM_25; // Available GPIO for lock relay control
#endif

// Constructor
BoardESP32::BoardESP32()
#ifdef ENABLE_LOCK_RELAY
    : Board(PinRX, PinTX, PinRTS, PinLockRelay)
#else
    : Board(PinRX, PinTX, PinRTS)
#endif
{
  // Nothing to do
}

// Initializes the board
void BoardESP32::Init()
{
#ifdef ENABLE_LOCK_RELAY
  // Initialize lock relay pin (active HIGH to unlock)
  pinMode(PinLockRelay, OUTPUT);
  digitalWrite(PinLockRelay, LOW); // Default to locked
#endif
}

// Logs board name/information
void BoardESP32::Print()
{
  Logger::Print("ESP32");
}