#pragma once

// Pin connections:
// ESP32 GPIO18 -> MAXRS485 RO (Receiver Output)
// ESP32 GPIO19 -> MAXRS485 DI (Driver Input)
// ESP32 GPIO21 -> MAXRS485 RO
constexpr uint8_t PIN_RX = GPIO_NUM_18;
constexpr uint8_t PIN_TX = GPIO_NUM_19;
constexpr uint8_t PIN_RTS = GPIO_NUM_21;