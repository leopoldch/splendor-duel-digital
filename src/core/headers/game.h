#ifndef LO21_SPLENDOR_DUEL_GAME_H
#define LO21_SPLENDOR_DUEL_GAME_H
#include "json.h"
#include <iostream>
class StrategyPlayer;

class Game {
  private:
	bool is_finished = false;
	StrategyPlayer *who_plays;
	unsigned int round = 0;
	StrategyPlayer *opponent;
	vector<const JewelryCard *> jewelry_cards; // contains all cards of the game
	vector<const Token *> tokens;              // contains all tokens
	vector<const RoyalCard *> royal_cards;
	vector<const Privilege *> privileges;
	Deck *deck1;
	Deck *deck2;
	Deck *deck3;

	Draw *draw1;
	Draw *draw2;
	Draw *draw3;

	struct Handler {
		Game *instance = nullptr;
		~Handler() {
			delete instance;
			instance = nullptr;
		}
	};

	static Handler handler;
	Game();
	Game(json data);
	~Game();

	Game(const Game &) = delete;
	Game &operator=(const Game &) = delete;

  public:
	json toJson() const;

	// Initialize player names
	void setPlayers(string &name1, string &name2, string &user_player_choice1,
	                string &user_player_choice2); // version Qt
	void setPlayers();

	void fillBoard() { Board::getBoard().fill(Bag::get()); }

	StrategyPlayer &getCurrentPlayer() const { return *who_plays; }
	StrategyPlayer &getOpponent() const { return *opponent; }

	vector<const RoyalCard *> getRoyalCards() const { return royal_cards; }

	vector<const JewelryCard *> getJewelryCards() const {
		return jewelry_cards;
	}

	const RoyalCard &drawRoyalCard(unsigned int i) {
		if (i > royal_cards.size() || i < 0)
			throw SplendorException("Invalid index!");
		const RoyalCard *tmp = royal_cards[i];
		royal_cards.erase(royal_cards.begin() + i);
		return *tmp;
	}

	const unsigned int getPrivilegeNumber() const { return privileges.size(); }

	const Privilege *getPrivilege() {
		if (privileges.size() <= 0) {
			throw SplendorException("No more privileges available");
		}
		const Privilege *tmp = privileges[0];
		privileges.erase(privileges.begin());
		return tmp;
	}

	Deck *getDeck(int num) const {
		if (num == 1)
			return deck1;
		if (num == 2)
			return deck2;
		if (num == 3)
			return deck3;

		throw SplendorException("This deck does not exists!");
	}

	void setPrivilege(const Privilege &p) {
		if (privileges.size() >= 3) {
			throw SplendorException(
			    "Maximum number of privileges in the game already reached");
		}
		privileges.push_back(&p);
	}

	void printRoyalCards();

	const bool isFinished();

	StrategyPlayer &getPlayerRound();
	Draw *getFirstDraw() { return draw1; }
	Draw *getSecondDraw() { return draw2; }
	Draw *getThirdDraw() { return draw3; }
	Draw *getDrawById(int i) {
		switch (i) {
		case (1):
			return draw1;
			break;
		case (2):
			return draw2;
			break;
		case (3):
			return draw3;
			break;
		default:
			throw SplendorException("The requested draw does not exist!");
		}
	}
	void nextRound(bool replay = 0);
	static Game &getGame();
	static Game &getGame(json data);
	static void free();
	static void test();

	const Token *getToken(int i) { return tokens[i]; }
	void setPlayerNames(const string &s1, const string &s2);

	vector<const Privilege *> &getPrivileges() { return privileges; }
	unsigned int getRoundCount() { return round; }
};

#endif // LO21_SPLENDOR_DUEL_GAME_H
