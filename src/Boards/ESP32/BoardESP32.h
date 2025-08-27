#pragma once

class BoardESP32 : public Board
{
public:
  // Constructor
  BoardESP32();

  // Initializes the board
  virtual void Init();

  // Logs board name/information
  virtual void Print();
};