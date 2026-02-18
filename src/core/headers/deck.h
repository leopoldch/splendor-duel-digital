#ifndef LO21_SPLENDOR_DUEL_DECK_H
#define LO21_SPLENDOR_DUEL_DECK_H
#include "card.h"
#include <algorithm>
#include <iostream>
#include <random>

using namespace std;

class Deck {
  protected:
	static const int max_decks = 3;
	static int decks_number;

  private:
	const int level;

	Deck &operator=(Deck &p) = delete;

	vector<const JewelryCard *> deck;

  public:
	~Deck() { decks_number--; }

	Deck(int niv) : level(niv) {
		if (decks_number > max_decks) {
			throw SplendorException("Maximum number of decks reached");
		}
		++decks_number;
	}

	int getLevel() const { return level; }

	bool isEmpty() { return deck.empty(); }

	vector<const JewelryCard *> getDeck() const { return deck; }

	static int getDecksNumber() { return decks_number; }

	const void setDeck(vector<const JewelryCard *> p) { deck = p; }

	static void initDecks(Deck *deck1, Deck *deck2, Deck *deck3,
	                      vector<const JewelryCard *> &cards);

	const JewelryCard &getCard();

	const json toJson() {
		json j;
		j["level"] = getLevel();
		j["jewelry_cards"] = {};
		for (int i = 0; i < deck.size(); ++i) {
			j["jewelry_cards"].push_back(deck[i]->toJson());
		}
		return j;
	}
};

#endif // LO21_SPLENDOR_DUEL_MAIN_H
