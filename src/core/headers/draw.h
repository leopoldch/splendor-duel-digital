#ifndef LO21_SPLENDOR_DUEL_DRAW_H
#define LO21_SPLENDOR_DUEL_DRAW_H
#include "Exception.h"
#include "card.h"
#include "deck.h"
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

class Draw {

	static const int max_draw_amount = 3; // draw limit
	static int draw_count;                // counter
	Deck &deck;
	const int level;
	int cards_number;
	const int max_cards;
	vector<const JewelryCard *> cards;

  public:
	const json toJson() {
		json j;
		j["level"] = getLevel();
		j["cards_number"] = cards_number;
		j["max_cards"] = max_cards;
		j["jewelry_cards"] = {};

		for (int i = 0; i < cards.size(); ++i) {
			j["jewelry_cards"].push_back(cards[i]->toJson());
		}

		return j;
	}

	// draw constructor
	Draw(int level, int max_cards, Deck &deck)
	    : level(level), max_cards(max_cards), deck(deck) {
		if (deck.getLevel() != level) {
			throw SplendorException(
			    "The deck is not the same level as the draw!");
		} else if (draw_count == max_draw_amount) {
			throw SplendorException("Maximum number of draws exceeded!");
		}

		cards_number = 0; // initialize the number of cards to 0
		draw_count++;
	}

	~Draw() { draw_count--; }

	// getters definition
	const int getLevel() const { return level; }
	const int getCardsNumber() const { return cards_number; }
	void setCardCount(int nb) { cards_number = nb; }

	Deck &getDeck() const { return deck; }
	vector<const JewelryCard *> &getDrawCards() { return cards; }

	void setDrawCards(vector<const JewelryCard *> draw) { cards = draw; }

	// declaration of the method to fill the Draw with cards (see draw.cpp)
	void fill();

	const JewelryCard &getCard(unsigned int index) {

		if (cards_number == 0) {
			throw SplendorException("Warning: no cards in the draw!");
		}

		const JewelryCard *return_card = cards[index];
		cards.erase(cards.begin() + index);
		cards_number--;

		return *return_card;
	}

	const JewelryCard &getCardWithoutDeletion(unsigned int index) {
		if (cards_number == 0) {
			throw SplendorException("Warning: no cards in the draw!");
		}
		return *cards[index];
	}

  private:
	Draw &operator=(const Draw &draw) = delete;
	Draw(const Draw &draw) = delete;
};

// operator overload to print the draw to standard output
inline std::ostream &operator<<(std::ostream &f, Draw &draw) {
	int i = 0;
	for (const JewelryCard *c : draw.getDrawCards()) {
		f << "index : " << i << " card : " << *c << endl;
		i++;
	}
	return f;
}

void testDraw();

#endif // LO21_SPLENDOR_DUEL_DRAW_H
