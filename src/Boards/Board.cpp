#include <Arduino.h>
#include "Board.h"

// Constructor
#ifdef ENABLE_LOCK_RELAY
Board::Board(uint8_t pinRx, uint8_t pinTx, uint8_t pinRts, uint8_t pinLockRelay)
    : mPinRx(pinRx), mPinTx(pinTx), mPinRts(pinRts), mPinLockRelay(pinLockRelay)
{
}
#else
Board::Board(uint8_t pinRx, uint8_t pinTx, uint8_t pinRts)
    : mPinRx(pinRx), mPinTx(pinTx), mPinRts(pinRts)
{
}
#endif

// These functions return the pins used by this board
uint8_t Board::GetPinRx()
{
  return mPinRx;
}

// These functions return the pins used by this board
uint8_t Board::GetPinTx()
{
  return mPinTx;
}

// These functions return the pins used by this board
uint8_t Board::GetPinRts()
{
  return mPinRts;
}

#ifdef ENABLE_LOCK_RELAY
// These functions return the pins used by this board
uint8_t Board::GetPinLockRelay()
{
  return mPinLockRelay;
}
#endif