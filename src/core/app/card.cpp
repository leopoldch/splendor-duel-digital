#include "card.h"
#include <fstream>
int Card::cards_number = 0;

std::string toString(Color c) {
	switch (c) {
	case Color::red:
		return "Red";
	case Color::blue:
		return "Blue";
	case Color::green:
		return "Green";
	case Color::white:
		return "White";
	case Color::black:
		return "Black";
	case Color::perl:
		return "Pearl";
	case Color::gold:
		return "Gold";
	default:
		throw SplendorException("Unknown color");
	}
}
std::string toString(optional<Capacity> c) {
	if (c != nullopt) {
		if (c == Capacity::replay) {
			return "Replay";
		} else if (c == Capacity::steal_opponent_pawn) {
			return "Steal an opponent's pawn";
		} else if (c == Capacity::take_priviledge) {
			return "Take a privilege";
		} else if (c == Capacity::take_on_board) {
			return "Take a pawn of the same color as the card bonus on the "
			       "board";
		} else if (c == Capacity::joker) {
			return "Joker";
		} else {
			throw SplendorException("Unknown capacity");
		}
	} else {
		return "Null";
	}
}

std ::string toString(optional<enum colorBonus> bonus) {
	if (bonus != nullopt) {
		if (bonus == colorBonus::blue) {
			return "Bonus Blue";
		} else if (bonus == colorBonus::red) {
			return "Bonus Red";
		} else if (bonus == colorBonus::green) {
			return "Bonus Green";
		} else if (bonus == colorBonus::white) {
			return "Bonus White";
		} else if (bonus == colorBonus::black) {
			return "Bonus Black";
		} else if (bonus == colorBonus::joker) {
			return "Bonus Joker";
		}
		return "Unknown Bonus";
	} else {
		return "Null";
	}
}

std::string toStringJson(Color c) {
	switch (c) {
	case Color::red:
		return "red";
	case Color::blue:
		return "red";
	case Color::green:
		return "green";
	case Color::white:
		return "white";
	case Color::black:
		return "black";
	case Color::perl:
		return "perl";
	case Color::gold:
		return "gold";
	default:
		throw SplendorException("Unknown color");
	}
}

std::string toStringJson(optional<Capacity> c) {
	if (c != nullopt) {
		if (c == Capacity::replay) {
			return "replay";
		} else if (c == Capacity::steal_opponent_pawn) {
			return "steal_opponent_pawn";
		} else if (c == Capacity::take_priviledge) {
			return "take_priviledge";
		} else if (c == Capacity::take_on_board) {
			return "take_on_board";
		} else if (c == Capacity::joker) {
			return "joker";
		}
		return "unknown";
	} else {
		return "null";
	}
}

std ::string toStringJson(optional<enum colorBonus> bonus) {
	if (bonus != nullopt) {
		if (bonus == colorBonus::blue) {
			return "blue";
		} else if (bonus == colorBonus::red) {
			return "red";
		} else if (bonus == colorBonus::green) {
			return "green";
		} else if (bonus == colorBonus::white) {
			return "white";
		} else if (bonus == colorBonus::black) {
			return "black";
		} else if (bonus == colorBonus::joker) {
			return "joker";
		}
		return "unknown";
	} else {
		return "null";
	}
}

std::ostream &operator<<(std::ostream &f, optional<Capacity> c) {
	if (c == nullopt)
		return f << " Null ";
	return f << toString(c);
}

std::ostream &operator<<(std::ostream &f, Color c) {
	f << toString(c);
	return f;
}

std::ostream &operator<<(ostream &f, optional<enum colorBonus> b) {
	f << toString(b);
	return f;
}

const std::initializer_list<Color> Colors = {
    Color::red, Color::blue, Color::green, Color::white, Color::black};

const std::initializer_list<Capacity> Capacities = {
    Capacity::replay, Capacity::steal_opponent_pawn, Capacity::take_priviledge,
    Capacity::take_on_board, Capacity::joker};

const std::initializer_list<enum colorBonus> Color_bonus = {
    colorBonus::blue,  colorBonus::red,   colorBonus::green,
    colorBonus::white, colorBonus::black, colorBonus::joker};

std::map<std::string, enum colorBonus> bonusMap = {
    {"blue", colorBonus::blue},   {"red", colorBonus::red},
    {"green", colorBonus::green}, {"white", colorBonus::white},
    {"black", colorBonus::black}, {"joker", colorBonus::joker}};

optional<enum colorBonus> getBonusFromString(const std::string &str) {
	auto it = bonusMap.find(str);
	if (it != bonusMap.end()) {
		return it->second;
	} else {
		return nullopt;
	}
}

std::map<std::string, Capacity> capacityMap = {
    {"replay", Capacity::replay},
    {"steal_opponent_pawn", Capacity::steal_opponent_pawn},
    {"take_priviledge", Capacity::take_priviledge},
    {"take_on_board", Capacity::take_on_board},
    {"joker", Capacity::joker}};

optional<Capacity> getCapacityFromString(const std::string &str) {
	auto it = capacityMap.find(str);
	if (it != capacityMap.end()) {
		return it->second;
	} else {
		return nullopt;
	}
}

vector<const RoyalCard *> initRoyalCards() {
	std::ifstream file("../src/cards.json");

	if (!file.is_open()) {
		throw SplendorException("Failed to open the JSON file.");
	}

	json data;
	file >> data;
	file.close();

	std::vector<const RoyalCard *> royal_cards_instances;

	for (const auto &royal_card_data : data["royal_cards"]) {

		std::string image = royal_card_data["link"];
		std::string visual = "../src/assets/cards_detoured/" + image;

		if (!royal_card_data["capacity"].is_null()) {
			RoyalCard *instance = new RoyalCard(
			    royal_card_data["prestige_points"],
			    getCapacityFromString(royal_card_data["capacity"]), visual);
			royal_cards_instances.push_back(instance);
		} else {
			RoyalCard *instance = new RoyalCard(
			    royal_card_data["prestige_points"], nullopt, visual);
			royal_cards_instances.push_back(instance);
		}
	}
	return royal_cards_instances;
}

vector<const JewelryCard *> initJewelryCards() {
	std::ifstream file("../src/cards.json");

	if (!file.is_open()) {
		throw SplendorException("Failed to open the JSON file.");
	}

	json data;
	file >> data;
	file.close();
	std::vector<const JewelryCard *> jewelry_cards_instances;
	for (const auto &jewelry_card_data : data["jewelry_cards"]) {

		std::string image = jewelry_card_data["link"];
		std::string visual = "../src/assets/cards_detoured/" + image;

		if (jewelry_card_data["capacity"].is_null() &&
		    jewelry_card_data["color_bonus"].is_null()) {

			JewelryCard *instance = new JewelryCard(
			    jewelry_card_data["prestige_points"],
			    jewelry_card_data["white_cost"], jewelry_card_data["blue_cost"],
			    jewelry_card_data["red_cost"], jewelry_card_data["green_cost"],
			    jewelry_card_data["black_cost"], jewelry_card_data["perl_cost"],
			    jewelry_card_data["level"], jewelry_card_data["crown_number"],
			    jewelry_card_data["bonus_number"], nullopt, nullopt, visual);
			jewelry_cards_instances.push_back(instance);
		} else if (!jewelry_card_data["capacity"].is_null() &&
		           jewelry_card_data["color_bonus"].is_null()) {

			JewelryCard *instance = new JewelryCard(
			    jewelry_card_data["prestige_points"],
			    jewelry_card_data["white_cost"], jewelry_card_data["blue_cost"],
			    jewelry_card_data["red_cost"], jewelry_card_data["green_cost"],
			    jewelry_card_data["black_cost"], jewelry_card_data["perl_cost"],
			    jewelry_card_data["level"], jewelry_card_data["crown_number"],
			    jewelry_card_data["bonus_number"], nullopt,
			    getCapacityFromString(jewelry_card_data["capacity"]), visual);
			jewelry_cards_instances.push_back(instance);

		} else if (jewelry_card_data["capacity"].is_null() &&
		           !jewelry_card_data["color_bonus"].is_null()) {

			JewelryCard *instance = new JewelryCard(
			    jewelry_card_data["prestige_points"],
			    jewelry_card_data["white_cost"], jewelry_card_data["blue_cost"],
			    jewelry_card_data["red_cost"], jewelry_card_data["green_cost"],
			    jewelry_card_data["black_cost"], jewelry_card_data["perl_cost"],
			    jewelry_card_data["level"], jewelry_card_data["crown_number"],
			    jewelry_card_data["bonus_number"],
			    getBonusFromString(jewelry_card_data["color_bonus"]), nullopt,
			    visual);
			jewelry_cards_instances.push_back(instance);
		} else {

			JewelryCard *instance = new JewelryCard(
			    jewelry_card_data["prestige_points"],
			    jewelry_card_data["white_cost"], jewelry_card_data["blue_cost"],
			    jewelry_card_data["red_cost"], jewelry_card_data["green_cost"],
			    jewelry_card_data["black_cost"], jewelry_card_data["perl_cost"],
			    jewelry_card_data["level"], jewelry_card_data["crown_number"],
			    jewelry_card_data["bonus_number"],
			    getBonusFromString(jewelry_card_data["color_bonus"]),
			    getCapacityFromString(jewelry_card_data["capacity"]), visual);
			jewelry_cards_instances.push_back(instance);
		}
	}
	return jewelry_cards_instances;
}

json JewelryCard::toJson() const {
	json j;
	if (getCapacity() != nullopt) {
		j["capacity"] = toStringJson(getCapacity());
	} else {
		j["capacity"] = NULL;
	}
	j["prestige_points"] = getPrestige();
	j["level"] = getLevel();
	if (getBonus() != nullopt) {
		j["color_bonus"] = toStringJson(getBonus());
	} else {
		j["color_bonus"] = NULL;
	}
	j["bonus_number"] = getNbBonus();
	j["crown_number"] = getNbCrown();
	j["white_cost"] = getCostWhite();
	j["blue_cost"] = getCostBlue();
	j["green_cost"] = getCostGreen();
	j["red_cost"] = getCostRed();
	j["black_cost"] = getCostBlack();
	j["perl_cost"] = getCostPerl();
	j["visual"] = getVisual();

	return j;
}
json RoyalCard::toJson() const {
	json j;
	if (getCapacity() != nullopt) {
		j["capacity"] = toStringJson(getCapacity());
	} else {
		j["capacity"] = NULL;
	}
	j["prestige_points"] = getPrestige();
	j["visual"] = getVisual();

	return j;
}

void testInitCards() {

	vector<const RoyalCard *> test = initRoyalCards();
	vector<const JewelryCard *> test2 = initJewelryCards();

	for (int k = 0; k < test2.size(); k++) {
		cout << *test2[k] << endl;
	}
	for (int k = 0; k < test.size(); k++) {
		cout << *test[k] << endl;
	}

	cout << "\n" << Card::getCardsNumber() << endl;
}

const Color colorBonusToColor(const optional<colorBonus> &color) {
	if (color == colorBonus::red)
		return Color::red;
	if (color == colorBonus::green)
		return Color::green;
	if (color == colorBonus::blue)
		return Color::blue;
	if (color == colorBonus::white)
		return Color::white;
	if (color == colorBonus::black)
		return Color::black;
	else
		throw SplendorException("Unknown bonus color!");
}

const colorBonus stringToBonus(const std::string &str) {
	if (str == "Red")
		return colorBonus::red;
	if (str == "Green")
		return colorBonus::green;
	if (str == "Blue")
		return colorBonus::blue;
	if (str == "White")
		return colorBonus::white;
	if (str == "Black")
		return colorBonus::black;
	else
		throw SplendorException("Unknown bonus color string!");
}
