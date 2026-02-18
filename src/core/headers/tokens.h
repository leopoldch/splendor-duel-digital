#ifndef LO21_SPLENDOR_DUEL_TOKENS_H
#define LO21_SPLENDOR_DUEL_TOKENS_H
#include "Exception.h"
#include "card.h"
#include <iostream>
#include <vector>

using namespace std;

class Token {
	const int id;
	const Color color;

	static int perl_number;
	static const int max_perl = 2;
	static int gold_number;
	static const int max_gold = 3;
	static int green_number;
	static const int max_green = 4;
	static int red_number;
	static const int max_red = 4;
	static int blue_number;
	static const int max_blue = 4;
	static int white_number;
	static const int max_white = 4;
	static int black_number;
	static const int max_black = 4;

	const std::string visual;

  public:
	Token(int id, const Color &c, std::string visual)
	    : id(id), color(c), visual(visual) {

		if ((id < 1) || (id > Token::getMaxTokenNumber())) {
			throw SplendorException("Bad token id!");
		}

		switch (c) {
		case Color::red: {
			if (red_number < max_red) {
				red_number++;
			} else {
				throw SplendorException(
				    "Cannot create more red tokens!");
			}
			break;
		}
		case Color::blue: {
			if (blue_number < max_blue) {
				blue_number++;
			} else {
				throw SplendorException(
				    "Cannot create more blue tokens!");
			}
			break;
		}
		case Color::green: {
			if (green_number < max_green) {
				green_number++;
			} else {
				throw SplendorException(
				    "Cannot create more green tokens!");
			}
			break;
		}
		case Color::white: {
			if (white_number < max_white) {
				white_number++;
			} else {
				throw SplendorException(
				    "Cannot create more white tokens!");
			}
			break;
		}
		case Color::black: {
			if (black_number < max_black) {
				black_number++;
			} else {
				throw SplendorException(
				    "Cannot create more black tokens!");
			}
			break;
		}
		case Color::perl: {
			if (perl_number < max_perl) {
				perl_number++;
			} else {
				throw SplendorException(
				    "Cannot create more pearl tokens!");
			}
			break;
		}
		case Color::gold: {
			if (gold_number < max_gold) {
				gold_number++;
			} else {
				throw SplendorException(
				    "Cannot create more gold tokens!");
			}
			break;
		}
		default: {
			throw SplendorException("Unknown color");
			break;
		}
		}
	}
	~Token() {
		switch (color) {
		case Color::red: {
			if (red_number > 0) {
				red_number--;
			}
			break;
		}
		case Color::blue: {
			if (blue_number > 0) {
				blue_number--;
			}
			break;
		}
		case Color::green: {
			if (green_number > 0) {
				green_number--;
			}
			break;
		}
		case Color::white: {
			if (white_number > 0) {
				white_number--;
			}
			break;
		}
		case Color::black: {
			if (black_number > 0) {
				black_number--;
			}
			break;
		}
		case Color::perl: {
			if (perl_number > 0) {
				perl_number--;
			}
			break;
		}
		case Color::gold: {
			if (gold_number > 0) {
				gold_number--;
			}
			break;
		}
		default: {
			break;
		}
		}
	}
	Token &operator=(const Token &tok) = delete;
	Token(const Token &tok) = delete;

	const std::string getVisual() const { return visual; }

	const Color &getColor() const { return color; }
	const int getId() const { return id; }
	static int getMaxTokenNumber() {
		return max_blue + max_white + max_gold + max_black + max_perl +
		       max_red + max_green;
	}

	static void resetCounters() {
		red_number = 0;
		blue_number = 0;
		green_number = 0;
		white_number = 0;
		black_number = 0;
		perl_number = 0;
		gold_number = 0;
	}

	json toJson(const int place = -1) const {
		json j;
		j["id"] = getId();
		j["color"] = getColor();
		j["visual"] = getVisual();
		return j;
	}
};

inline ostream &operator<<(ostream &f, const Token &tok) {
	f << "id: " << tok.getId() << "; color: " << toString(tok.getColor());
	return f;
}

vector<const Token *> initTokens();

#endif // LO21_SPLENDOR_DUEL_TOKENS_H
