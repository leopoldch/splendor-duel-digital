#ifndef LO21_SPLENDOR_DUEL_CARD_H
#define LO21_SPLENDOR_DUEL_CARD_H
#include "Exception.h"
#include <array>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

using namespace std;

enum class Capacity {
	replay,
	steal_opponent_pawn,
	take_priviledge,
	take_on_board,
	joker
};

enum class Color { blue, red, green, white, black, perl, gold };
enum class colorBonus { blue, red, green, white, black, joker };

optional<Capacity> getCapacityFromString(const std::string &str);
optional<colorBonus> getBonusFromString(const std::string &str);

std::string toString(Color c);
std::string toString(Capacity c);
std ::string toString(optional<colorBonus> bonus);

std::string toStringJson(Color c);
std::string toStringJson(Capacity c);
std ::string toStringJson(optional<colorBonus> bonus);

std::ostream &operator<<(std::ostream &f, Color c);
std::ostream &operator<<(std::ostream &f, optional<Capacity> c);
std::ostream &operator<<(ostream &f, optional<colorBonus> b);

extern const std::initializer_list<Color> Colors;
extern const std::initializer_list<Capacity> Capacities;
extern const std::initializer_list<colorBonus> ColorBonuses;

class Card {

  protected:
	static const int MAX = 71;
	static int cards_number;

  private:
	Card &operator=(Card &c) = delete;

	const optional<Capacity> capacity;
	const int prestige_points;

	const std::string visual;

  public:
	static int getCardsNumber() { return cards_number; }

	virtual ~Card() { cards_number--; }

	explicit Card(int prestige_points = 0,
	              std::optional<Capacity> capacity = std::nullopt,
	              std::string visual = "")
	    : capacity(capacity), prestige_points(prestige_points), visual(visual) {
		if (cards_number > MAX) {
			throw SplendorException("Maximum cards reached");
		}

		if (prestige_points < 0 || prestige_points > 10) {
			throw SplendorException("Unauthorized value");
		}
		++cards_number;
	}

	const optional<Capacity> &getCapacity() const { return capacity; }

	const std::string getVisual() const { return visual; }

	const int getPrestige() const { return prestige_points; }
};

class RoyalCard : public Card {

	static const int max_royal_card = 4;

  public:
	json toJson() const;

	static const int getMaxRoyalCard() { return max_royal_card; }

	RoyalCard(int prestige_points = 0,
	          std::optional<Capacity> capacity = nullopt,
	          std::string visual = "")
	    : Card(prestige_points, capacity, visual) {
		if (prestige_points < 0 || prestige_points > 10) {
			throw SplendorException("Unauthorized value");
		}
		if (cards_number > MAX) {
			throw SplendorException("Maximum cards reached");
		}
	}
};

class JewelryCard : public Card {
	const int white_cost;
	const int blue_cost;
	const int red_cost;
	const int green_cost;
	const int black_cost;
	const int perl_cost;
	const int level;
	const int crown_count;
	mutable optional<enum colorBonus> bonus;
	const int bonus_number;

	static const int max_jewelry_card = 67;

  public:
	json toJson() const;

	const int getCostWhite() const { return white_cost; }
	const int getCostBlue() const { return blue_cost; }
	const int getCostRed() const { return red_cost; }
	const int getCostGreen() const { return green_cost; }
	const int getCostBlack() const { return black_cost; }
	const int getCostPerl() const { return perl_cost; }
	static const int getMaxJewelryCard() { return max_jewelry_card; }

	JewelryCard(int points_prestiges = 0, int white_cost = 0, int blue_cost = 0,
	            int red_cost = 0, int green_cost = 0, int black_cost = 0,
	            int perl_cost = 0, int level = 1, int crown_count = 0,
	            int bonus_number = 0, optional<enum colorBonus> bonus = nullopt,
	            optional<Capacity> capacity = nullopt, std::string visual = "")
	    : Card(points_prestiges, capacity, visual), white_cost(white_cost),
	      blue_cost(blue_cost), red_cost(red_cost), green_cost(green_cost),
	      black_cost(black_cost), perl_cost(perl_cost), level(level),
	      crown_count(crown_count), bonus(bonus), bonus_number(bonus_number) {
		if (cards_number > MAX) {

			throw SplendorException("Maximum cards reached");
		}
		if (white_cost < 0 || white_cost > 10 || blue_cost < 0 ||
		    blue_cost > 10 || red_cost < 0 || red_cost > 10 || green_cost < 0 ||
		    green_cost > 10 || black_cost < 0 || black_cost > 10 ||
		    perl_cost < 0 || perl_cost > 10 || level < 1 || level > 3 ||
		    crown_count < 0 || crown_count > 5 || bonus_number < 0 ||
		    bonus_number > 5) {
			throw SplendorException("Unauthorized value");
		}
	}

	const int getLevel() const { return level; }
	const int getNbCrown() const { return crown_count; }
	const optional<enum colorBonus> &getBonus() const { return bonus; }
	const int getNbBonus() const { return bonus_number; }
	void changeBonusColor(colorBonus b) const { bonus = b; }
};

inline std::ostream &operator<<(std::ostream &os, const Card &c) {
	os << "Capacity : " << c.getCapacity()
	   << " prestige points : " << c.getPrestige() << endl;
	return os;
}
inline std::ostream &operator<<(std::ostream &os, const RoyalCard &c) {
	os << "Capacity : " << c.getCapacity()
	   << ", prestige points : " << c.getPrestige() << endl;
	return os;
}
inline std::ostream &operator<<(std::ostream &os, const JewelryCard &c) {

	os << "Capacity : " << c.getCapacity();
	if (c.getPrestige() > 0) {
		os << ", prestige points : " << c.getPrestige();
	}
	if (c.getCostPerl() > 0) {
		os << ", Pearl cost :" << c.getCostPerl();
	}
	if (c.getCostWhite() > 0) {
		os << ", White cost :" << c.getCostWhite();
	}
	if (c.getCostBlue() > 0) {
		os << ", Blue cost :" << c.getCostBlue();
	}
	if (c.getCostRed() > 0) {
		os << ", Red cost :" << c.getCostRed();
	}
	if (c.getCostGreen() > 0) {
		os << ", Green cost :" << c.getCostGreen();
	}
	if (c.getCostBlack() > 0) {
		os << ", Black cost :" << c.getCostBlack();
	}
	if (c.getNbBonus() > 0) {
		os << ", Bonus : " << c.getBonus()
		   << ", Bonus amount : " << c.getNbBonus();
	}
	if (c.getNbCrown() > 0) {
		os << ", Crown amount : " << c.getNbCrown();
	}
	os << ", Level : " << c.getLevel() << endl;

	return os;
}

inline Color stringToColor(std::string &str) {
	if (str == "Blue")
		return Color::blue;
	if (str == "White")
		return Color::white;
	if (str == "Green")
		return Color::green;
	if (str == "Red")
		return Color::red;
	if (str == "Black")
		return Color::black;
	if (str == "Pearl")
		return Color::perl;
	if (str == "Gold")
		return Color::gold;
	else
		throw SplendorException("The string passed does not match any color! ");
}
const Color colorBonusToColor(const optional<enum colorBonus> &color);
const colorBonus stringToBonus(const std::string &str);
vector<const RoyalCard *> initRoyalCards();
vector<const JewelryCard *> initJewelryCards();

void testInitCards();
#endif // LO21_SPLENDOR_DUEL_CARD_H
