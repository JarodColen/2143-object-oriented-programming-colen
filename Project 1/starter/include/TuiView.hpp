#pragma once

#include "Blackjack.hpp"
#include "GameView.hpp"

// FTXUI terminal interface.
class TuiView : public GameView {
private:
  Blackjack &game;

public:
  explicit TuiView(Blackjack &game);
  void run() override;
};
