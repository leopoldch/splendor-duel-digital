#include "draw.h"

int Draw::draw_count = 0; // counter initialization to 0

void Draw::fill() {

	vector<const JewelryCard *> p = this->deck.getDeck();

	while (this->cards_number < this->max_cards && !this->deck.isEmpty()) {

		const JewelryCard &c = this->deck.getCard();
		this->cards.push_back(&c);
		this->cards_number++;
	}
}

void testDraw() {

	Deck deck1 = Deck(1);
	Deck deck2 = Deck(2);
	Deck deck3 = Deck(3);

	vector<const JewelryCard *> card_list = initJewelryCards();

	deck1.initDecks(&deck1, &deck2, &deck3, card_list);

	std::cout << deck1.getDeck().size() << endl;

	Draw draw1 = Draw(1, 5, deck1);
	draw1.fill();

	std::cout << deck1.getDeck().size() << endl;

	Draw draw2 = Draw(2, 4, deck2);
	draw2.fill();

	Draw draw3 = Draw(3, 3, deck3);
	draw3.fill();

	Draw draw4 = Draw(3, 3, deck3);
	draw4.fill();

	draw1.fill();
}
