#include "bag.h"

Bag::BagHandler Bag::bag_handler;

Bag &Bag::get() {
	if (bag_handler.instance == nullptr) {
		bag_handler.instance = new Bag();
	}
	return *bag_handler.instance;
}

void Bag::free() {
	delete bag_handler.instance;
	bag_handler.instance = nullptr;
}

void Bag::insertToken(const Token *tok) {
	tokens.push_back(tok);
	tokens_in_bag_number++;
}

void Bag::printBag() {
	for (int i = 0; i < Token::getMaxTokenNumber(); i++) {
		cout << *tokens[i] << endl;
	}
}
