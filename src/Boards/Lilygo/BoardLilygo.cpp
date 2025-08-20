#include <Arduino.h>
#include "Components/Logger/Logger.h"
#include "../Board.h"
#include "BoardLilygo.h"

// Pin connections:
// ESP32 GPIO21 -> MAXRS485 RO (Receiver Output)
// ESP32 GPIO22 -> MAXRS485 DI (Driver Input)
// ESP32 GPIO21 -> MAXRS485 DE+RE
constexpr uint8_t PinRX = GPIO_NUM_21;
constexpr uint8_t PinTX = GPIO_NUM_22;
constexpr uint8_t PinRTS = GPIO_NUM_21;

// Additional pins
constexpr uint8_t Pin_5V_EN = 16;
constexpr uint8_t Pin_CAN_TX = 26;
constexpr uint8_t Pin_CAN_RX = 27;
constexpr uint8_t Pin_CAN_SE = 23;
constexpr uint8_t Pin_RS485_EN = 17;
constexpr uint8_t Pin_RS485_TX = 22;
constexpr uint8_t Pin_RS485_RX = 21;
constexpr uint8_t Pin_RS485_SE = 19;
constexpr uint8_t Pin_SD_MISO = 2;
constexpr uint8_t Pin_SD_MOSI = 15;
constexpr uint8_t Pin_SD_SCLK = 14;
constexpr uint8_t PinSD_CS = 13;
constexpr uint8_t Pin_WS2812 = 4;

// Constructor
BoardLilygo::BoardLilygo()
    : Board(PinRX, PinTX, PinRTS)
{
  // Nothing to do
}

// Initializes the board
void BoardLilygo::Init()
{
  pinMode(Pin_RS485_EN, OUTPUT);
  digitalWrite(Pin_RS485_EN, HIGH);

  pinMode(Pin_RS485_SE, OUTPUT);
  digitalWrite(Pin_RS485_SE, HIGH);

  pinMode(Pin_5V_EN, OUTPUT);
  digitalWrite(Pin_5V_EN, HIGH);
}

// Logs board name/information
void BoardLilygo::Print()
{
  Logger::Print("LILYGO T-CAN485");
}