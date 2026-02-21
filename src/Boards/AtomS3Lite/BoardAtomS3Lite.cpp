#include <Arduino.h>
#include "Components/Logger/Logger.h"
#include "../Board.h"
#include "BoardAtomS3Lite.h"

// Pin connections:
// TX -> GPIO6
// RX -> GPIO5
// RTS -> GPIO6
constexpr uint8_t PinRX = GPIO_NUM_5;
constexpr uint8_t PinTX = GPIO_NUM_6;
constexpr uint8_t PinRTS = GPIO_NUM_6;

// Constructor
BoardAtomS3Lite::BoardAtomS3Lite()
    : Board(PinRX, PinTX, PinRTS)
{
  // Nothing to do
}

// Initializes the board
void BoardAtomS3Lite::Init()
{
  // No extra pins to initialize for Atom S3 Lite
}

// Logs board name/information
void BoardAtomS3Lite::Print()
{
  Logger::Print("ATOM S3 LITE");
}
