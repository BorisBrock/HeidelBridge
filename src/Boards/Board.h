#pragma once

class Board
{
protected:
  // Constructor
#ifdef ENABLE_LOCK_RELAY
  Board(uint8_t pinRx, uint8_t pinTx, uint8_t pinRts, uint8_t pinLockRelay);
#else
  Board(uint8_t pinRx, uint8_t pinTx, uint8_t pinRts);
#endif

public:
  // Initializes the board
  virtual void Init() = 0;

  // Logs board name/information
  virtual void Print() = 0;

  // These functions return the pins used by this board
  uint8_t GetPinRx();
  uint8_t GetPinTx();
  uint8_t GetPinRts();
#ifdef ENABLE_LOCK_RELAY
  uint8_t GetPinLockRelay();
#endif

private:
  uint8_t mPinRx, mPinTx, mPinRts;
#ifdef ENABLE_LOCK_RELAY
  uint8_t mPinLockRelay;
#endif
};