#include <Arduino.h>
#include "Components/Logger/Logger.h"
#include "../Board.h"
#include "BoardOlimex.h"

// Olimex ESP32-POE-WROVER + MAX485 via UEXT
// UEXT pin 1: 3.3V            -> MAX485 VCC
// UEXT pin 2: GND             -> MAX485 GND
// UEXT pin 3: GPIO4  (U1TXD) -> MAX485 DI
// UEXT pin 4: GPIO36 (U1RXD) <- MAX485 RO
// UEXT pin 5: GPIO33          -> MAX485 DE + RE
constexpr uint8_t PinRX = GPIO_NUM_36;
constexpr uint8_t PinTX = GPIO_NUM_4;
constexpr uint8_t PinRTS = GPIO_NUM_33;

// Constructor
BoardOlimex::BoardOlimex()
    : Board(PinRX, PinTX, PinRTS)
{
  // Nothing to do
}

// Initializes the board
void BoardOlimex::Init()
{
  // Nothing to do
}

// Logs board name/information
void BoardOlimex::Print()
{
  Logger::Print("Olimex ESP32-POE-WROVER + MAX485 via UEXT");
  Logger::Print("  RS485 RX: GPIO36 (UEXT pin 4)");
  Logger::Print("  RS485 TX: GPIO4  (UEXT pin 3)");
  Logger::Print("  RS485 RTS: GPIO33 (UEXT pin 5)");
}
