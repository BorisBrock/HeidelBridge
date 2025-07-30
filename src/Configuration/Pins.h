#pragma once

namespace Pins
{
#ifdef LILYGO_BOARD
    // Config for the Lilygo T-Can485 Board
    // Pin connections:
    // ESP32 GPIO21 -> MAXRS485 RO (Receiver Output)
    // ESP32 GPIO22 -> MAXRS485 DI (Driver Input)
    // ESP32 GPIO21 -> MAXRS485 DE+RE
    constexpr uint8_t PinRX = GPIO_NUM_21;
    constexpr uint8_t PinTX = GPIO_NUM_22;
    constexpr uint8_t PinRTS = GPIO_NUM_21;
#else
    // Config for regular ESP32 boards + external RS485 module
    // Pin connections:
    // ESP32 GPIO18 -> MAXRS485 RO (Receiver Output)
    // ESP32 GPIO19 -> MAXRS485 DI (Driver Input)
    // ESP32 GPIO21 -> MAXRS485 DE+RE
    constexpr uint8_t PinRX = GPIO_NUM_18;
    constexpr uint8_t PinTX = GPIO_NUM_19;
    constexpr uint8_t PinRTS = GPIO_NUM_21;
#endif
};

#ifdef LILYGO_BOARD
#define PIN_5V_EN 16

#define CAN_TX_PIN 26
#define CAN_RX_PIN 27
#define CAN_SE_PIN 23

#define RS485_EN_PIN 17 // 17 /RE
#define RS485_TX_PIN 22 // 21
#define RS485_RX_PIN 21 // 22
#define RS485_SE_PIN 19 // 22 /SHDN

#define SD_MISO_PIN 2
#define SD_MOSI_PIN 15
#define SD_SCLK_PIN 14
#define SD_CS_PIN 13

#define WS2812_PIN 4
#endif
