#include <Arduino.h>
#include "Board.h"
#include "BoardFactory.h"
#include "../Configuration/Settings.h"

// All supported boards
#include "ESP32/BoardESP32.h"
#include "Lilygo/BoardLilygo.h"

// Factory instance
BoardFactory *BoardFactory::Instance()
{
  static BoardFactory factoryInstance;
  return &factoryInstance;
}

// Gets the currently used board
Board *BoardFactory::GetBoard()
{
  static BoardESP32 genericBoard;
  static BoardLilygo lilygoBoard;

  if (Settings::Instance()->BoardType == "lilygo")
  {
    return &lilygoBoard;
  }
  return &genericBoard;
}