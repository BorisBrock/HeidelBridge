#pragma once

// Forward declarations
class Board;

class BoardFactory
{
private:
  BoardFactory() {};

public:
  // Factory instance
  static BoardFactory *Instance();

  // Gets the currently used board
  static Board *GetBoard();
};