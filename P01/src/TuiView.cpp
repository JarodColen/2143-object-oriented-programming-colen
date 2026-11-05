// this is the pretty looking version of the game
// using ftxui

#include "TuiView.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <string>

using namespace ftxui;

// draws one card face-up
// red or white for color
// gets the symbols for rank and suit
// and formats them
static Element cardElement(const Card &c) {
  Color col = c.isRed() ? Color::Red : Color::White;
  return (text(c.rankSymbol() + c.suitSymbol()) | center |
          size(WIDTH, EQUAL, 6) | size(HEIGHT, EQUAL, 3)) |
        color(col) | bold | border;
}

// for the hole / hidden card
// displays "??" on the hidden card
static Element hiddenCardElement() {
  return (text("??") | center | size(WIDTH, EQUAL, 6) |
          size(HEIGHT, EQUAL, 3)) |
        color(Color::Blue) | border;
}

// puts all the cards in a horizontal box, in a row
static Element handElement(const Hand &h, bool hideSecond) {
  Elements row;
  for (size_t i = 0; i < h.size(); ++i)
    row.push_back(hideSecond && i == 1 ? hiddenCardElement()
                                      : cardElement(h[i]));
  return hbox(row);
}

// color based on the win or loss
// green for win
// red for loss
// yellow for other / tie
static Color outcomeColor(Outcome o) {
  switch (o) {
  case Outcome::PlayerBlackjack:
  case Outcome::PlayerWin:
  case Outcome::DealerBust: return Color::Green;
  case Outcome::DealerWin:
  case Outcome::PlayerBust: return Color::Red;
  default:                  return Color::Yellow;
  }
}

TuiView::TuiView(Blackjack &game) : game(game) {}

void TuiView::run() {
  auto screen = ScreenInteractive::Fullscreen();

  std::string betText; // player input
  std::string notice;  

  auto renderer = Renderer([&] {
    bool hidden = game.dealerHoleHidden();
    const Hand &p = game.playerHand(); // p = player's hand
    const Hand &d = game.dealerHand(); // d = dealer's hand
    Phase ph = game.phase();

    // Dealer's total is never shown while the hole card is hidden.
    // if dealer has no cards, no text
    // or if hole card is hidden, show "?"
    // otherwise show the real total
    std::string dealerTotal =
        d.empty() ? "" : (hidden ? "?" : std::to_string(Blackjack::handValue(d)));

    // if player has no cards, show no text
    // otherwise show total 
    // and if total is soft, add "(soft)" next to total
    std::string playerTotal =
        p.empty() ? ""
                  : std::to_string(Blackjack::handValue(p)) +
                        (Blackjack::isSoft(p) ? " (soft)" : "");


    // this is the header above the card area
    // displaying the chips and bet amount
    Elements body;
    body.push_back(
        hbox({text("Chips: " + std::to_string(game.chips())) | bold,
              filler(),
              text(ph == Phase::Betting || ph == Phase::GameOver
                       ? ""
                       : "Bet: " + std::to_string(game.bet())) |
                  bold}));
    body.push_back(separator());

    // this is the visual information in the game body
    // if the dealer has cards in their hand 
    // show the cards and hand amount text
    // else, the bet text will be shown in its place
    if (!d.empty()) {
      body.push_back(text("Dealer  " + dealerTotal) | bold);              // total will be either ? or a number. text is bolded
      body.push_back(handElement(d, hidden));                             // dealer's cards and have them hidden or not
      body.push_back(text(""));                                           // just spacing
      body.push_back(text("You  " + playerTotal) | bold);                 // Player's current total
      body.push_back(handElement(p, false));                              // player's cards
    } else {
      body.push_back(text("Place your bet to start the round.") | dim);   // text shown to place bets before round if hand is empty
    }
    body.push_back(separator());                                          // line seperator under the body
    

    // in betting phase, it'll show the bet amount and the area to enter your bet
    // it also show which keys are to be pressed
    // during the player turn, it simply shows what buttons you can press
    // round over, it'll do the same but with the outcome text above, and then the enter to next round or quit
    // game over, just like round over, but all the player can do now is quit
    switch (ph) {
    case Phase::Betting:
      body.push_back(text("Bet (1-" + std::to_string(game.chips()) +
                          "): " + betText + "_") |
                     bold);
      body.push_back(text("type digits   a all-in   Enter deal   q quit") |
                     dim);
      break;
    case Phase::PlayerTurn:
      body.push_back(text("h hit    s stand    q quit") | dim);
      break;
    case Phase::RoundOver:
      body.push_back(text(Blackjack::outcomeName(game.outcome())) | bold |
                     color(outcomeColor(game.outcome())));
      body.push_back(text("Enter next round    q quit") | dim);
      break;
    case Phase::GameOver:
      body.push_back(text("GAME OVER - you're out of chips") | bold |
                     color(Color::Red));
      body.push_back(text("q quit") | dim);
      break;
    }
    if (!notice.empty())
      body.push_back(text(notice) | color(Color::Yellow));

    return vbox({text("BLACKJACK") | bold | center, vbox(body) | border}) | center;   // this is the header above the game
  });

  // this part handles the key inputs, some based on the current phase of the game
  auto app = CatchEvent(renderer, [&](Event e) {
    if (e == Event::Character('q')) {             // this takes the input at any point to close the game with 'q'
      screen.Exit();
      return true;
    }

    switch (game.phase()) {
    case Phase::Betting:
      if (e.is_character()) {
        char ch = e.character()[0];
        if (ch >= '0' && ch <= '9') {
          if (betText.size() < 9)
            betText += e.character();
          notice.clear();
          return true;
        }
      }
      if (e == Event::Backspace) {
        if (!betText.empty())
          betText.pop_back();
        return true;
      }
      if (e == Event::Character('a')) {
        betText = std::to_string(game.chips());
        notice.clear();
        return true;
      }
      if (e == Event::Return) {
        if (betText.empty() || !game.startRound(std::stoi(betText)))                // if the bet chips amount is empty or if its an invalid input
          notice = "Bet must be between 1 and " + std::to_string(game.chips());     // like 0 chips or an amount over the max amount of chips the player has
        else {
          notice.clear();
          betText.clear();
        }
        return true;
      }
      break;

    case Phase::PlayerTurn:
      if (e == Event::Character('h')) {
        game.hit();
        return true;
      }
      if (e == Event::Character('s')) {
        game.stand();
        return true;
      }
      break;

    case Phase::RoundOver:
      if (e == Event::Return) {
        game.nextRound();
        return true;
      }
      break;

    case Phase::GameOver:
      break;
    }
    return false;
  });

  screen.Loop(app);
}
