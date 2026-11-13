#pragma once

// FreeRTOS mutex mock: the host tests run single-threaded, so taking and giving always succeed.

#include "FreeRTOS.h"

typedef void *SemaphoreHandle_t;

inline SemaphoreHandle_t xSemaphoreCreateMutex()
{
    static int token;
    return &token;
}

inline int xSemaphoreTake(SemaphoreHandle_t mutex, TickType_t ticksToWait)
{
    return 1;
}

inline int xSemaphoreGive(SemaphoreHandle_t mutex)
{
    return 1;
}
