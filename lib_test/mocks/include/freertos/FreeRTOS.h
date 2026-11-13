#pragma once

// Minimal FreeRTOS mock for host-based unit tests.

#include <cstdint>

typedef uint32_t TickType_t;
#define portMAX_DELAY 0xFFFFFFFFu
