#pragma once

class BoardLilygo : public Board
{
public:
  // Constructor
  BoardLilygo();

  // Initializes the board
  virtual void Init();

  // Logs board name/information
  virtual void Print();
};