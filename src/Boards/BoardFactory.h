#pragma once

// Forward declarations
class IBoard;

class BoardFactory
{
private:
  BoardFactory() {};

public:
  // Factory instance
  static BoardFactory *Instance();

  // Gets the currently used board
  static IBoard *GetBoard();
};