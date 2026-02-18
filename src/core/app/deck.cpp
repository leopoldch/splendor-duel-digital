#include "deck.h"

int Deck::decks_number = 0;

const JewelryCard &Deck::getCard() {
	if (this->deck.size() == 0) {
		throw SplendorException("Warning: no cards in the deck!");
	}
	const JewelryCard *return_card = deck[0];
	deck.erase(deck.begin());
	return *return_card;
}

void Deck::initDecks(Deck *deck1, Deck *deck2, Deck *deck3,
                     vector<const JewelryCard *> &cards) {
	if (decks_number != max_decks) {
		throw SplendorException("not all decks have been created");
	}

	for (const JewelryCard *card : cards) { // for each card (constant ptr
		                                    // to a card) in the cards vector
		switch (card->getLevel()) {
		case 1:
			(deck1->deck).push_back(card);
			break;
		case 2:
			(deck2->deck).push_back(card);
			break;
		case 3:
			(deck3->deck).push_back(card);
			break;
		}
	}
	// Shuffle the deck elements
	random_device rd;
	mt19937 g(rd());

	shuffle((deck1->deck).begin(), (deck1->deck).end(), g);
	shuffle((deck2->deck).begin(), (deck2->deck).end(), g);
	shuffle((deck3->deck).begin(), (deck3->deck).end(), g);
}
