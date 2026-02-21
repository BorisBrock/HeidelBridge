#include <Arduino.h>
#include "Board.h"
#include "BoardFactory.h"

// All supported boards
#include "AtomS3Lite/BoardAtomS3Lite.h"
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
#ifdef BOARD_ATOM_S3_LITE
  static BoardAtomS3Lite boardInstance;
#elif BOARD_ESP32
  static BoardESP32 boardInstance;
#elif BOARD_LILYGO
  static BoardLilygo boardInstance;
#endif

  return &boardInstance;
}
