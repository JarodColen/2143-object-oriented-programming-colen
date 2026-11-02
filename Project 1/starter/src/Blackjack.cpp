#include "Blackjack.hpp"

// new game with startingChips (100 by default)
// begin with betting phase
// sets phase to GameOver if chips is 0 (or less)
Blackjack::Blackjack(int startingChips)
    : currentPhase(startingChips > 0 ? Phase::Betting : Phase::GameOver),
      currentOutcome(Outcome::None), chipCount(startingChips), currentBet(0),
      holeHidden(false) {}

// the value in points of the cards
// each value is basically increased by 1 from its code
// ace is in slot zero, worth 1. 2 in slot 1, worth 2. etc.
// and all the face cards are worth 10
static int cardPoints(const Card &c) {
  int r = c.getRank(); 
  if (r == 0) // ace is rank 0, worth 1 by default
    return 1;
  if (r >= 9) // all cards higher than 9 (rank 8) are worth 10 
    return 10;
  return r + 1; // every other card gets its value here
}

// adds up the whole hand (ace is counted as 1 here)
// also informs the function if an ace is in hand
static int hardTotal(const Hand &h, bool &hasAce) {
  int total = 0;
  hasAce = false;
  for (const Card &c : h) {
    total += cardPoints(c);
    if (c.getRank() == 0)
      hasAce = true;
  }
  return total;
}

// like hardTotal
// also uses hasAce to determine the value
int Blackjack::handValue(const Hand &h) {
  bool hasAce;
  int total = hardTotal(h, hasAce);
  if (hasAce && total + 10 <= 21) // changes ace from 1 to 11 if it doesn't bust
    total += 10;
  return total;
}

// "soft" is when an ace is valued at 11
bool Blackjack::isSoft(const Hand &h) {
  bool hasAce;
  int total = hardTotal(h, hasAce);
  return hasAce && total + 10 <= 21;
}

// natural is when 21 is reached in only 2 cards
bool Blackjack::isNatural(const Hand &h) {
  return h.size() == 2 && handValue(h) == 21;
}

// makes the round outcome into text for display
std::string Blackjack::outcomeName(Outcome o) {
  switch (o) {
  case Outcome::None:            return "In progress";
  case Outcome::PlayerBlackjack: return "Blackjack! You win (3:2)"; // aka "natural"
  case Outcome::PlayerWin:       return "You win";
  case Outcome::DealerWin:       return "Dealer wins";
  case Outcome::Push:            return "Push (tie)";
  case Outcome::PlayerBust:      return "You bust";
  case Outcome::DealerBust:      return "Dealer busts, you win";
  }
  return "";
}

void Blackjack::dealTo(Hand &h) { h.push_back(deck.deal()); }

// adjust the chip count based on the outcomes
void Blackjack::settle(Outcome o) {
  currentOutcome = o;
  holeHidden = false;
  switch (o) {
    case Outcome::PlayerBlackjack: chipCount += currentBet * 3 / 2; break; // a natural gets a *3/2 multiplier in blackjack (rounding down)
    case Outcome::PlayerWin:
    case Outcome::DealerBust:      chipCount += currentBet; break;
    case Outcome::DealerWin:
    case Outcome::PlayerBust:      chipCount -= currentBet; break;
    default:                       break; // also for push
  }
  currentPhase = Phase::RoundOver; // ends round
}

// starts the round
// first checks if its the betting phase, otherwise returns false
// then checks if you have enough chips to play, otherwise returns false
// then saves the player's bet
// it resets the deck and shuffles it fresh
// sets the Phase to the player's turn
// and if its a natural right away it will end the round
bool Blackjack::startRound(int bet) {
  if (currentPhase != Phase::Betting)
    return false;
  if (bet < 1 || bet > chipCount)
    return false;

  currentBet = bet;
  currentOutcome = Outcome::None;
  player.clear();
  dealer.clear();

  deck.reset();
  deck.shuffle();

  // player, dealer, player, dealer
  // Dealer's 2nd card is the hole card.
  dealTo(player);
  dealTo(dealer);
  dealTo(player);
  dealTo(dealer);
  holeHidden = true;
  currentPhase = Phase::PlayerTurn;

  // Naturals are checked right after the deal
  bool pNat = isNatural(player);
  bool dNat = isNatural(dealer);
  if (pNat && dNat)
    settle(Outcome::Push);
  else if (pNat)
    settle(Outcome::PlayerBlackjack);
  else if (dNat)
    settle(Outcome::DealerWin);

  return true;
}

// deals a card to the player
// checks if the value is over 21 and either continues or busts
bool Blackjack::hit() {
  if (currentPhase != Phase::PlayerTurn)
    return false;
  dealTo(player);
  if (handValue(player) > 21)
    settle(Outcome::PlayerBust); // dealer does not play
  return true;
}

// reveals the hole card from the dealer
// then the dealer draws cards until it reaches 17 or higher
// and compares the totals to see if its a win, bust, or push (tie)
bool Blackjack::stand() {
  if (currentPhase != Phase::PlayerTurn)
    return false;

  holeHidden = false; // reveal the hole card
  while (handValue(dealer) < 17) // hits on 16 or less, stands on 17+
    dealTo(dealer);

  int p = handValue(player);  // value of the player's hand
  int d = handValue(dealer);  // value of the dealer's hand
  if (d > 21)
    settle(Outcome::DealerBust);
  else if (p > d)
    settle(Outcome::PlayerWin);
  else if (p < d)
    settle(Outcome::DealerWin);
  else
    settle(Outcome::Push);
  return true;
}

// round must be over to move to the next round
// uses the total chip amount to figure out if the game is over
// or if its time to bet again
bool Blackjack::nextRound() {
  if (currentPhase != Phase::RoundOver)
    return false;
  currentPhase = (chipCount <= 0) ? Phase::GameOver : Phase::Betting;
  return true;
}


// getters to get values
Phase Blackjack::phase() const { return currentPhase; }
int Blackjack::chips() const { return chipCount; }
int Blackjack::bet() const { return currentBet; }
const Hand &Blackjack::playerHand() const { return player; }
const Hand &Blackjack::dealerHand() const { return dealer; }
bool Blackjack::dealerHoleHidden() const { return holeHidden; }
Outcome Blackjack::outcome() const { return currentOutcome; }