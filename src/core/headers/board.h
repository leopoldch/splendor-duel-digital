#ifndef LO21_SPLENDOR_DUEL_BOARD_H
#define LO21_SPLENDOR_DUEL_BOARD_H
#include "Exception.h"
#include "bag.h"
#include "tokens.h"
#include <iostream>
#include <vector>

const int order[25] = {12, 7,  8,  13, 18, 17, 16, 11, 6,  1,  2, 3, 4,
                       9,  14, 19, 24, 23, 22, 21, 20, 15, 10, 5, 0};
enum class Position {
	right,
	diag_upper_right,
	above,
	diag_upper_left,
	left,
	diag_lower_left,
	below,
	diag_lower_right
};
extern const std::initializer_list<Position> Positions;

optional<Position> invertPositions(const optional<Position> &pos);
std::string toString(optional<Position> &pos);

using namespace std;

class Board {
	/*
	 * We use the following board layout, according to the order of tokens
	 * in the array:    0   |  1   |   2  |   3  |  4
	 *   --------------------------------
	 *     5   |  6   |  7   |   8  |  9
	 *   --------------------------------
	 *     10  |  11  |  12  |  13  |  14
	 *   --------------------------------
	 *     15  |  16  |  17  |  18  |  19
	 *   --------------------------------
	 *     20  |  21  |  22  |  23  |  24
	 */

	int nb = 0;
	int current_nb = 0;

	vector<const Token *> tokens;
	struct BoardHandler {
		Board *instance = nullptr;
		~BoardHandler() {
			delete instance;
			instance = nullptr;
		}
	};
	static BoardHandler board_handler;
	Board() : nb(Token::getMaxTokenNumber()), tokens() {
		for (size_t i = 0; i < Token::getMaxTokenNumber(); i++) {
			tokens.push_back(nullptr);
		}
	};
	~Board() = default; // aggregation!
	Board &operator=(const Board &p) = delete;
	Board(const Board &p) = delete;

  public:
	json toJson() const {
		json j;
		j["tokens"] = {};
		j["nb"] = nb;
		j["current_nb"] = current_nb;
		for (int i = 0; i < tokens.size(); ++i) {
			if (tokens[i] != nullptr) {
				j["tokens"].push_back(tokens[i]->toJson(i));
			} else {
				j["tokens"].push_back(nullptr);
			}
		}
		return j;
	}

	const int getCurrentNb() const { return current_nb; }
	const int getNb() const { return nb; }

	void setCurrentNb(const int value) { current_nb = value; }

	static Board &getBoard();
	static void free();

	const bool isInside(const unsigned int uid) {
		for (int i = 0; i < tokens.size(); ++i) {
			if (tokens[i]->getId() == uid) {
				return true;
			}
		}
		return false;
	}

	unsigned int getIndex(const unsigned int uid) {
		for (int i = 0; i < tokens.size(); ++i) {
			if (tokens[i] != nullptr and tokens[i]->getId() == uid) {
				return i;
			}
		}
		throw SplendorException("Token not present on the board");
	}

	void setTokens(vector<const Token *> j) {
		if (tokens.size() > nb) {
			throw SplendorException("Too many tokens in the array");
		}
		tokens = j;
	}

	void setNb(unsigned int n) { nb = n; }

	const Token *getBoardCaseByIndex(int i) const { return tokens[i]; }

	void setTokenOnBoardByIndex(int i, const Token *tok) { tokens[i] = tok; }

	const Token *getElementToTheRight(int i) const {
		if ((i + 1) % 5 != 0) {
			return tokens[++i];
		} else {
			return nullptr;
		}
	}
	const Token *getElementToTheLeft(int i) const {
		if (i % 5 != 0) {
			return tokens[--i];
		} else {
			return nullptr;
		}
	}
	const Token *getElementAbove(int i) const {
		if (i > 4) {
			return tokens[i - 5];
		} else {
			return nullptr;
		}
	}
	const Token *getElementBelow(int i) const {
		if (i < 20) {
			return tokens[i + 5];
		} else {
			return nullptr;
		}
	}
	const Token *getElementToTheUpperRight(int i) const {
		if ((i > 4) && ((i + 1) % 5 != 0)) { // if the token is not on the upper level
			                                 // and on a right edge
			return tokens[i - 4];
		} else {
			return nullptr;
		}
	}
	const Token *getElementToTheUpperLeft(int i) const {
		if ((i > 4) && (i % 5 != 0)) { // if the token is not on the upper level
			                           // and on a left edge
			return tokens[i - 6];
		} else {
			return nullptr;
		}
	}

	const Token *getElementToTheBelowRight(int i) const {
		if ((i < 20) &&
		    ((i + 1) % 5 != 0)) { // if the token is not on the lower level
			                      // and on a right edge
			return tokens[i + 6];
		} else {
			return nullptr;
		}
	}

	const Token *getElementToTheBelowLeft(int i) const {
		if ((i < 20) && (i % 5 != 0)) { // if the token is not on the lower level
			                            // and on a right edge
			return tokens[i + 4];
		} else {
			return nullptr;
		}
	}

	optional<const Position> tokensAreASide(int i, const Token *tok);

	void fill(Bag &bag);
	void printArray() const;
	bool onlyGold();
	bool colorsOnBoard(const optional<enum colorBonus> &color);

	vector<int>
	getTokenIndexesByColor(const optional<enum colorBonus> &color);
};

#endif // LO21_SPLENDOR_DUEL_BOARD_H
