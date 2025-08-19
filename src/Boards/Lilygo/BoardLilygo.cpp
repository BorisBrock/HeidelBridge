#include <Arduino.h>
#include "../Board.h"
#include "BoardLilygo.h"

// Pin connections:
// ESP32 GPIO21 -> MAXRS485 RO (Receiver Output)
// ESP32 GPIO22 -> MAXRS485 DI (Driver Input)
// ESP32 GPIO21 -> MAXRS485 DE+RE
constexpr uint8_t PinRX = GPIO_NUM_21;
constexpr uint8_t PinTX = GPIO_NUM_22;
constexpr uint8_t PinRTS = GPIO_NUM_21;

// Constructor
BoardLilygo::BoardLilygo()
    : Board(PinRX, PinTX, PinRTS)
{
}