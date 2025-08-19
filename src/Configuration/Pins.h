#pragma once

namespace Pins
{
#ifdef BOARD_ESP32

    // Config for regular ESP32 boards
    // Pin connections:
    // ESP32 GPIO18 -> MAXRS485 RO (Receiver Output)
    // ESP32 GPIO19 -> MAXRS485 DI (Driver Input)
    // ESP32 GPIO21 -> MAXRS485 DE+RE
    constexpr uint8_t PinRX = GPIO_NUM_18;
    constexpr uint8_t PinTX = GPIO_NUM_19;
    constexpr uint8_t PinRTS = GPIO_NUM_21;

#elif BOARD_LILYGO

    // Config for the Lilygo T-Can485 board
    // Pin connections:
    // ESP32 GPIO21 -> MAXRS485 RO (Receiver Output)
    // ESP32 GPIO22 -> MAXRS485 DI (Driver Input)
    // ESP32 GPIO21 -> MAXRS485 DE+RE
    constexpr uint8_t PinRX = GPIO_NUM_21;
    constexpr uint8_t PinTX = GPIO_NUM_22;
    constexpr uint8_t PinRTS = GPIO_NUM_21;

#endif
};


MUSS WEG, SOLL IN BOARDS