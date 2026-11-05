#pragma once

#include "Blackjack.hpp"
#include "GameView.hpp"

class ConsoleView : public GameView {
private:
  Blackjack &game;

  void showTable() const;
  std::string handText(const Hand &h, bool hideSecond) const;
  bool readLine(std::string &line) const; // false on EOF

public:
  explicit ConsoleView(Blackjack &game);
  void run() override;
};
