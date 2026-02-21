#pragma once

#include <Arduino.h>
#include "../Board.h"

class BoardAtomS3Lite : public Board
{
public:
  BoardAtomS3Lite();

  // Initializes the board
  void Init() override;

  // Logs board name/information
  void Print() override;
};
