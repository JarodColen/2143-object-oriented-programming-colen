#pragma once

#include "Card.hpp"
#include "Deck.hpp"

#include <string>
#include <vector>

using Hand = std::vector<Card>;

enum class Phase { Betting, PlayerTurn, RoundOver, GameOver };
enum class Outcome {
  None,
  PlayerBlackjack,
  PlayerWin,
  DealerWin,
  Push,
  PlayerBust,
  DealerBust
};

class Blackjack {
private:
  Deck deck;
  Hand player;
  Hand dealer;
  Phase currentPhase;
  Outcome currentOutcome;
  int chipCount;
  int currentBet;
  bool holeHidden;

  static bool isNatural(const Hand &h);
  void dealTo(Hand &h);
  void settle(Outcome o);

public:
  Blackjack(int startingChips = 100);

  bool startRound(int bet); // betting here, then player's turn
  bool hit();               // only during Player's turn
  bool stand();             // dealer plays, round settled
  bool nextRound();         // round over

  Phase phase() const;
  int chips() const;
  int bet() const; // bet of the current round
  const Hand &playerHand() const;
  const Hand &dealerHand() const;
  bool dealerHoleHidden() const;
  Outcome outcome() const;

  static int handValue(const Hand &h);
  static bool isSoft(const Hand &h);
  static std::string outcomeName(Outcome o);
};
