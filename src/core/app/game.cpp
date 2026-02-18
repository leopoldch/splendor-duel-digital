#include "game.h"
#include "player.h"
#include <string>
#include <time.h>

// condition to check if the game is over at each turn?

void Game::nextRound(bool replay) {
	if (who_plays->victoryConditions()) {
		is_finished = true;
	} else {
		StrategyPlayer *temp = who_plays;
		who_plays = opponent;
		opponent = temp;
		if (!replay)
			round++;
	}
}

const bool Game::isFinished() { return is_finished; }

StrategyPlayer &Game::getPlayerRound() { return *who_plays; }

Game::Game() {
	Token::resetCounters();

	Bag::get();

	int j = 1;

	for (int i = 0; i < 4; i++) {
		auto *temp = new Token(j++, Color::blue,
		                       "../src/assets/rest_detoured/Jeton_bleu.png");
		tokens.push_back(temp);
		Bag::get().insertToken(temp);
	}

	for (int i = 0; i < 4; i++) {
		auto *temp = new Token(j++, Color::red,
		                       "../src/assets/rest_detoured/Jeton_rouge.png");
		tokens.push_back(temp);
		Bag::get().insertToken(temp);
	}
	for (int i = 0; i < 4; i++) {
		auto *temp = new Token(j++, Color::green,
		                       "../src/assets/rest_detoured/Jeton_vert.png");
		tokens.push_back(temp);
		Bag::get().insertToken(temp);
	}
	for (int i = 0; i < 4; i++) {
		auto *temp = new Token(j++, Color::white,
		                       "../src/assets/rest_detoured/Jeton_blanc.png");
		tokens.push_back(temp);
		Bag::get().insertToken(temp);
	}
	for (int i = 0; i < 4; i++) {
		auto *temp = new Token(j++, Color::black,
		                       "../src/assets/rest_detoured/Jeton_noir.png");
		tokens.push_back(temp);
		Bag::get().insertToken(temp);
	}
	for (int i = 0; i < 2; i++) {
		auto *temp = new Token(j++, Color::perl,
		                       "../src/assets/rest_detoured/Jeton_perle.png");
		tokens.push_back(temp);
		Bag::get().insertToken(temp);
	}
	for (int i = 0; i < 3; i++) {
		auto *temp = new Token(j++, Color::gold,
		                       "../src/assets/rest_detoured/Jeton_or.png");
		tokens.push_back(temp);
		Bag::get().insertToken(temp);
	}

	Board::getBoard();
	Board::getBoard().fill(Bag::get());

	// cards constructor
	jewelry_cards = initJewelryCards();
	royal_cards = initRoyalCards();

	// privileges constructor
	privileges = initPrivileges();

	// decks:
	deck1 = new Deck(1);
	deck2 = new Deck(2);
	deck3 = new Deck(3);
	deck1->initDecks(deck1, deck2, deck3, jewelry_cards);

	draw1 = new Draw(1, 5, *deck1);
	draw2 = new Draw(2, 4, *deck2);
	draw3 = new Draw(3, 3, *deck3);
	draw1->fill();
	draw2->fill();
	draw3->fill();
}

json Game::toJson() const {
	json j;

	j["is_finished"] = is_finished;
	j["round"] = round;

	j["deck1"] = deck1->toJson();
	j["deck2"] = deck2->toJson();
	j["deck3"] = deck3->toJson();
	j["draw1"] = draw1->toJson();
	j["draw2"] = draw2->toJson();
	j["draw3"] = draw3->toJson();

	j["who_plays"] = Game::getGame().getCurrentPlayer().toJson();
	j["opponent"] = Game::getGame().getOpponent().toJson();

	j["privileges"] = {};
	for (int i = 0; i < privileges.size(); ++i) {
		j["privileges"].push_back(privileges[i]->toJson());
	}
	j["privilege_number"] = privileges.size();

	j["royal_cards"] = {};
	for (int i = 0; i < royal_cards.size(); ++i) {
		j["royal_cards"].push_back(royal_cards[i]->toJson());
	}
	j["bag"] = Bag::get().toJson();
	j["board"] = Board::getBoard().toJson();

	return j;
}

Game::Handler Game::handler;

Game::~Game() {

	for (auto token : tokens) {
		delete token;
	}
	tokens.clear();

	for (auto royal_cards : royal_cards) {
		delete royal_cards;
	}
	royal_cards.clear();

	for (auto it : jewelry_cards) {
		delete it;
	}
	jewelry_cards.clear();

	for (auto privilege : privileges) {
		delete privilege;
	}
	privileges.clear();

	delete deck1;
	delete deck2;
	delete deck3;

	delete draw1;
	delete draw2;
	delete draw3;

	// Destroy bag
	Bag::free();

	// Destroy board
	Board::free();

	// Destroy player and its privileges and royal cards
	delete who_plays;
	delete opponent;
};

Game &Game::getGame() {
	if (handler.instance == nullptr)
		handler.instance = new Game;
	return *handler.instance;
}
Game &Game::getGame(json data) {
	if (handler.instance == nullptr)
		handler.instance = new Game(data);
	return *handler.instance;
}

void Game::free() {
	delete handler.instance;
	handler.instance = nullptr;
}

void Game::test() {
	if (handler.instance == nullptr)
		std::cout << "Test";
}

void Game::printRoyalCards() {
	int i = 0;
	for (auto c : royal_cards) {
		cout << "Index : " << i++ << ", " << *c << endl;
	}
}
