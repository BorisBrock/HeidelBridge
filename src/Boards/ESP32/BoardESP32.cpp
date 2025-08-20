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

// Constructor
BoardESP32::BoardESP32()
    : Board(PinRX, PinTX, PinRTS)
{
  // Nothing to do
}

// Initializes the board
void BoardESP32::Init()
{
  // Nothing to do
}

// Logs board name/information
void BoardESP32::Print()
{
  Logger::Print("ESP32");
}