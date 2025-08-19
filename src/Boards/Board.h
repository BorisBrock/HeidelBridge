#pragma once

class Board
{
protected:
  // Constructor
  Board(uint8_t pinRx, uint8_t pinTx, uint8_t pinRts);

public:
  // These functions return the pins used by this board
  uint8_t GetPinRx();
  uint8_t GetPinTx();
  uint8_t GetPinRts();

private:
  uint8_t mPinRx, mPinTx, mPinRts;
};