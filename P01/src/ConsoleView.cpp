#include "ConsoleView.hpp"

#include <iostream>

ConsoleView::ConsoleView(Blackjack &game) : game(game) {}

bool ConsoleView::readLine(std::string &line) const {
  return static_cast<bool>(std::getline(std::cin, line));
}

std::string ConsoleView::handText(const Hand &h, bool hideSecond) const {
  std::string s;
  for (size_t i = 0; i < h.size(); ++i) {
    if (i > 0)
      s += ' ';
    s += (hideSecond && i == 1) ? "[??]" : "[" + h[i].toString() + "]";
  }
  return s;
}

void ConsoleView::showTable() const {
  bool hidden = game.dealerHoleHidden();
  std::cout << "\n------------------------------\n";
  std::cout << "Chips: " << game.chips() << "   Bet: " << game.bet() << '\n';

  std::cout << "Dealer: " << handText(game.dealerHand(), hidden);
  if (!hidden)
    std::cout << "  (" << Blackjack::handValue(game.dealerHand()) << ")";
  std::cout << '\n';

  const Hand &p = game.playerHand();
  std::cout << "You:    " << handText(p, false) << "  ("
            << Blackjack::handValue(p)
            << (Blackjack::isSoft(p) ? ", soft" : "") << ")\n";
}

void ConsoleView::run() {
  std::cout << "=== BLACKJACK ===  (type q at any prompt to quit)\n";
  std::string line;

  while (true) {
    switch (game.phase()) {

    case Phase::Betting: {
      std::cout << "\nChips: " << game.chips() << ". Bet (1-" << game.chips()
                << ", or 'all'): ";
      if (!readLine(line) || line == "q")
        return;
      int bet = 0;
      if (line == "all") {
        bet = game.chips();
      } else {
        try {
          bet = std::stoi(line); // stoi means "string to int"
        } catch (const std::exception &) {
          std::cout << "Please enter a number.\n";
          break;
        }
      }
      if (!game.startRound(bet))
        std::cout << "Bet must be between 1 and " << game.chips() << ".\n";
      break;
    }

    case Phase::PlayerTurn: {
      showTable();
      std::cout << "(h)it or (s)tand? ";
      if (!readLine(line) || line == "q")
        return;
      if (line == "h" || line == "hit")
        game.hit();
      else if (line == "s" || line == "stand")
        game.stand();
      else
        std::cout << "Please type h or s.\n";
      break;
    }

    case Phase::RoundOver: {
      showTable();
      std::cout << ">> " << Blackjack::outcomeName(game.outcome()) << "\n";
      std::cout << "Press Enter to continue...";
      if (!readLine(line) || line == "q")
        return;
      game.nextRound();
      break;
    }

    case Phase::GameOver:
      std::cout << "\nYou're out of chips. GAME OVER.\n";
      return;
    }
  }
}
