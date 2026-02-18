#include "player.h"
#include "card.h"
#include "game.h"
#include "main_window.qt.h"

#include <QInputDialog>
#include <QMessageBox>

using namespace std;
/******************** Utility functions ********************/
int positiveOrNull(int x) {
	if (x < 0)
		return 0;
	return x;
}
/******************** Utility functions ********************/

/******************** StrategyPlayer ********************/

// constructor destructor
StrategyPlayer::StrategyPlayer(const string &name)
    : points_number(0), jewelry_cards_number(0), royal_cards_number(0),
      crown_number(0), privilege_number(0), name(name), token_number(0),
      reserved_jewelry_cards_number(0), games(0), games_won(0) {}

StrategyPlayer::~StrategyPlayer() {
	// Destroy royal cards
	for (auto royal_cards : royal_cards) {
		delete royal_cards;
	}
	royal_cards.clear();

	// Destroy privileges
	for (auto privilege : privileges) {
		delete privilege;
	}
	privileges.clear();
}

int StrategyPlayer::calculateBonus(enum colorBonus bonus) {
	int result = 0;
	for (auto c : bought_jewelry_cards) {
		if ((*c).getBonus() == bonus) {
			result += (*c).getNbBonus();
		}
	}
	return result;
}

int StrategyPlayer::TokenAmount(const Color &color) const {
	int result = 0;
	for (auto j = tokens.begin(); j != tokens.end(); ++j) {
		if ((*j)->getColor() == color)
			result++;
	}
	return result;
}

void StrategyPlayer::withdrawTokens(const Color &c, int val) {
	int tmp = val;
	std::vector<int> tmp_tab(0);
	for (int k = 0; k < tokens.size(); k++) {
		if ((tokens[k]->getColor() == c) && (tmp != 0)) {
			tmp_tab.push_back(k);
			tmp--;
		}
	}

	if (tmp > 0) {
		throw SplendorException("Not enough " + toString(c) +
		                        " tokens to remove more!");
	}
	for (int j = val - 1; j >= 0; j--) { // error!
		Bag::get().insertToken(tokens[tmp_tab[j]]);
		tokens.erase(tokens.begin() + tmp_tab[j]);
		token_number--;
	}
	for (auto j : tokens) {
		cout << *j << endl;
	}
}

// TODO: rename
void StrategyPlayer::reserveCard(Draw *t, const int index) {
	// reserve a card from a draw
	unsigned int count = 0;
	for (int i = 0; i < tokens.size(); ++i) {
		if (tokens[i]->getColor() == Color::gold)
			count++;
	}
	if (count == 0) {
		throw SplendorException(
		    "The player does not have a gold token!");
	}
	const JewelryCard &tmp = t->getCard(index);
	reserved_jewelry_cards.push_back(&tmp);
	reserved_jewelry_cards_number++;
	t->fill();
}

// TODO: rename
void StrategyPlayer::reserveCard(Deck *p) {
	// reserve the card at the top of the deck
	unsigned int count = 0;
	for (int i = 0; i < tokens.size(); ++i) {
		if (tokens[i]->getColor() == Color::gold)
			count++;
	}
	if (count == 0) {
		throw SplendorException(
		    "The player does not have a gold token!");
	}
	const JewelryCard &tmp = p->getCard();
	reserved_jewelry_cards.push_back(&tmp);
	reserved_jewelry_cards_number++;
}

void StrategyPlayer::drawToken(int i) {

	if (i > 24 || i < 0) {
		throw SplendorException("Invalid board index!");
	}

	const Token *tmp = Board::getBoard().getBoardCaseByIndex(i);
	if (tmp == nullptr) {
		throw SplendorException("Token already taken!");
	}
	tokens.push_back(tmp);
	Board::getBoard().setTokenOnBoardByIndex(i, nullptr);
	Board::getBoard().setCurrentNb(Board::getBoard().getCurrentNb() - 1);
	std::cout << "Token acquired; remaining tokens on the board: "
	          << Board::getBoard().getCurrentNb() << std::endl;
	token_number++;
}

void StrategyPlayer::obtainRoyalCard(unsigned int i) {
	// take a card from the game
	if (i > Game::getGame().getRoyalCards().size()) {
		throw SplendorException("Card not available");
	}

	const RoyalCard &tmp = Game::getGame().drawRoyalCard(i);
	royal_cards.push_back(&tmp);
	royal_cards_number++;
	points_number += tmp.getPrestige();

	Game::getGame().getCurrentPlayer().applyRoyalCapacity(
	    tmp, Game::getGame().getOpponent());
}

// Qt overload
void StrategyPlayer::obtainRoyalCard_qt(unsigned int i) {
	// take a card from the game
	if (i > Game::getGame().getRoyalCards().size()) {
		throw SplendorException("Card not available");
	}
	// if (royalCardEligibility() == false) throw SplendorException("Pas
	// eligible.");
	const RoyalCard &tmp = Game::getGame().drawRoyalCard(i);
	royal_cards.push_back(&tmp);
	// REMOVE FROM game
	royal_cards_number++;
	points_number += tmp.getPrestige();

	Game::getGame().getCurrentPlayer().applyRoyalCapacity_Qt(
	    tmp, Game::getGame().getOpponent());
}

bool StrategyPlayer::royalCardEligibility() {
	if (crown_number >= 3 and crown_number < 6 and royal_cards_number == 0) {
		return true;
	}

	else if (crown_number >= 6 and royal_cards_number == 1) {
		return true;
	}

	else {
		return false;
	}
}

bool StrategyPlayer::jokerCardEligibility() {
	unsigned int test = 0;
	for (auto card : bought_jewelry_cards) {
		// test on all cards, as soon as the player owns a card of a color
		// (bonus != nullopt) they can buy a card with a color bonus
		if ((*card).getBonus() != nullopt) {
			return true;
		}
	}
	return false;
}

void StrategyPlayer::obtainPrivilege() {
	// get a privilege from the privilege draw. (from the board or from your
	// opponent?) first check if there are privileges in the game:

	if (privilege_number == Privilege::getMaxInstance()) {
		throw SplendorException(
		    "You already have the maximum allowed number of privileges!");
	}
	if (Game::getGame().getPrivilegeNumber() == 0) {
		// if the opponent has all the privileges
		if (this == &Game::getGame().getCurrentPlayer()) {
			// if the player obtaining the privilege is the one whose turn it
			// is, take from the opponent
			Game::getGame().getOpponent().withdrawPrivilege();
		} else { // if it is the opponent of the one whose turn it is, remove
			     // the privilege from the one who is currently playing
			Game::getGame().getCurrentPlayer().withdrawPrivilege();
		}
	}
	if (Game::getGame().getPrivilegeNumber() == 0) {
		throw SplendorException("No more privileges!");
	}
	privileges.push_back(Game::getGame().getPrivilege());
	privilege_number++;
}

void StrategyPlayer::withdrawPrivilege() {
	if (privilege_number == 0) {
		throw SplendorException(
		    "You cannot remove a privilege from the player");
	}
	Game::getGame().setPrivilege(*privileges[0]);
	privileges.erase(privileges.begin());
	privilege_number--;
}

void StrategyPlayer::fillingBoard() {
	Game::getGame().fillBoard();
	Game::getGame().getOpponent().obtainPrivilege();
	Board::getBoard().printArray();
}

// utility method for main
bool StrategyPlayer::victoryConditions() {
	if (crown_number >= 10)
		return true;
	if (points_number >= 20)
		return true;

	// test on colors, if the player has 10 points for cards whose bonus is
	// the same color, then the player wins
	int white_points = 0;
	int blue_points = 0;
	int red_points = 0;
	int green_points = 0;
	int black_points = 0;
	for (auto card : bought_jewelry_cards) {
		optional<colorBonus> bonus = (*card).getBonus();
		if (bonus != nullopt) {
			if (bonus == colorBonus::white)
				white_points += card->getPrestige();
			if (bonus == colorBonus::blue)
				blue_points += card->getPrestige();
			if (bonus == colorBonus::red)
				red_points += card->getPrestige();
			if (bonus == colorBonus::green)
				green_points += card->getPrestige();
			if (bonus == colorBonus::black)
				black_points += card->getPrestige();
		}
	}
	if (white_points >= 10 || blue_points >= 10 || black_points >= 10 ||
	    red_points >= 10 || green_points >= 10) {
		return true;
	}

	// return false if nothing is met!

	return false;
}

void StrategyPlayer::printPlayer() {
	cout << "Player : " << name << endl;
	cout << "Number of privileges: " << privilege_number << endl;
	cout << "Number of points: " << points_number << endl;
	cout << "Number of crowns: " << crown_number << endl;
	cout << "Owned jewelry cards: " << endl;
	for (auto card : bought_jewelry_cards) {
		cout << *card << endl;
	}
	cout << "---------------------------------------" << endl;
	cout << "Reserved jewelry cards: " << endl;
	for (auto card : reserved_jewelry_cards) {
		cout << *card << endl;
	}
	cout << "---------------------------------------" << endl;
	cout << "Royal cards:" << endl;
	for (auto card : royal_cards) {
		cout << *card << endl;
	}
	cout << "---------------------------------------" << endl;
	cout << "Owned tokens: " << endl;
	for (auto tok : tokens) {
		cout << *tok << endl;
	}
}

int StrategyPlayer::getOptionalChoices() {
	/***Convention***
	This method returns:
	 - 0 if the player or the AI cannot do anything
	 - 1 if the player or the AI can only use a privilege,
	 - 2 if the player or the AI can only fill the board,
	 - 3 if the player or the AI can do both.
	****************/
	int nb_choices = 0;
	if (Game::getGame().getCurrentPlayer().getPrivilegeNumber() > 0) {
		nb_choices += 1;
	}
	if (Board::getBoard().getCurrentNb() < Token::getMaxTokenNumber() and
	    Bag::get().getTokenNumber() > 0) {
		nb_choices += 2;
	}
	return nb_choices;
}

void StrategyPlayer::withdrawWhiteToken() {
	int counter = 0;
	for (size_t i = 0; i < tokens.size(); ++i) {
		if (tokens[i]->getColor() == Color::white) {
			tokens.erase(tokens.begin() + counter);
			break;
		}
		counter++;
	}
	token_number--;
}

void StrategyPlayer::withdrawPerlToken() {
	int counter = 0;
	for (size_t i = 0; i < tokens.size(); ++i) {
		if (tokens[i]->getColor() == Color::perl) {
			tokens.erase(tokens.begin() + counter);
			break;
		}
		counter++;
	}
	token_number--;
}

/******************** Player ********************/

// constructor and destructor
Player::Player(const string &name) : StrategyPlayer(name) {}
Player::Player(const json data) : StrategyPlayer(data) {}

Player::~Player() {
	// Destroy royal cards
	for (auto royal_cards : royal_cards) {
		delete royal_cards;
	}
	royal_cards.clear();

	// Destroy privileges
	for (auto privilege : privileges) {
		delete privilege;
	}
	privileges.clear();
}

// Polymorphic methods
void Player::choice() {
	int tmp = 0;
	bool end_choice = 0;
	int nb_choice = 0;
	while (!end_choice) {
		try {
			nb_choice =
			    getOptionalChoices(); // check the convention on the
			                          // return in the method definition
			int i = 1;
			cout << "Available optional actions:" << endl;
			if (nb_choice % 2 == 1) {
				cout << "Use a privilege -> " << i++ << endl;
			}
			if (nb_choice >= 2) {
				cout << "Fill the board -> " << i++ << endl;
			}
			cout << "Mandatory actions:" << endl;
			cout << "To take tokens press -> " << i++ << endl;
			cout << "To buy a card press -> " << i++ << endl;
			cout << "choices :";
			cin >> tmp;

			if (tmp < 1 or tmp > i + 1) {
				throw SplendorException(
				    "There are only " + to_string(i) +
				    " choices! You cannot choose anything else!\n");
			}
			string info;
			cout << "Do you validate your choice? [Y/N]" << endl;
			cin >> info;
			if (info == "N") {
				cout << "You did not validate, you must restart your choice!";
				throw SplendorException("");
			}

			switch (nb_choice) { // the display and therefore the choice depends on the
				                 // return value of the optional choices
			case (0): {          // no optional choice possible
				switch (tmp) {
				case 1: {
					tokenSelection();
					end_choice = 1;
					break;
				}
				case 2: {
					cardPurchase();
					end_choice = 1;

					break;
				}
				default: // continue until the user chooses
				         // a valid entry!
					break;
				}
				break;
			}
			case (1): { // only possible to use a privilege
				switch (tmp) {
				case 1: {
					usePrivilege();
					break;
				}
				case 2: {
					tokenSelection();
					end_choice = 1;
					break;
				}
				case 3: {
					cardPurchase();
					end_choice = 1;
					break;
				}
					//                        case 4: {
					//                            // display of the player's tokens
					//                            player!
					//                            //cout<<"Inventaire du player
					//                            :
					//                            "<<Game::getGame().getCurrentPlayer().getName()<<endl;
					//                            // display for each type
					//                            break;
					//                        }
				default:
					break;
				}
				break;
			}
			case (2): { // only possible to fill the board
				switch (tmp) {
				case 1: { // fill board
					fillingBoard();
					break;
				}
				case 2: {
					tokenSelection();
					end_choice = 1;
					break;
				}
				case 3: {
					cardPurchase();
					end_choice = 1;
					break;
				}
					//                        case 4: {
					//                            // display of the player's tokens
					//                            player!
					//                            //cout<<"Inventaire du player
					//                            :
					//                            "<<Game::getGame().getCurrentPlayer().getName()<<endl;
					//                            // display for each type
					//                            break;
					//                        }
				default:
					break;
				}
				break;
			}
			case (3): { // two optional choices possible
				switch (tmp) {
				case 1: {
					usePrivilege();
					break;
				}
				case 2: { // fill board
					fillingBoard();
					break;
				}
				case 3: {
					tokenSelection();
					end_choice = 1;
					break;
				}
				case 4: {
					cardPurchase();
					end_choice = 1;
					break;
				}
					//                        case 5: {
					//                            // display of the player's tokens
					//                            player!
					//                            //cout<<"Inventaire du player
					//                            :
					//                            "<<Game::getGame().getCurrentPlayer().getName()<<endl;
					//                            // display for each type
					//                            break;
					//                        }
				default:
					break;
				}
				break;
			}
			default:
				break;
			}

		} catch (SplendorException &e) {
			cout << e.getInfo() << "\n";
		}
	}
}

// Qt overload
void Player::choice_Qt() {

	int tmp = 0;
	bool end_choice = 0;
	int nb_choice = 0;
	char info;
	while (!end_choice) {
		try {
			MainWindow::getMainWindow().deactivateButtons();
			nb_choice =
			    getOptionalChoices(); // check the convention on the
			                          // return in the method definition
			MainWindow::getMainWindow().triggerNextAction(&tmp, this);

			if (info == 'N') {
				cout << "You did not validate, you must restart your choice!";
				throw SplendorException("");
			}

			switch (nb_choice) { // the display and therefore the choice depends on the
				                 // return value of the optional choices
			case (0): {          // no optional choice possible
				switch (tmp) {
				case 1: {
					tokenSelection_Qt();

					end_choice = 1;
					break;
				}
				case 2: {
					cardPurchase_Qt();

					end_choice = 1;

					break;
				}
				default: { // continue until the user chooses
					       // a valid entry!
					break;
				} break;
				}
				break;
			}
			case (1): { // only possible to use a privilege
				switch (tmp) {
				case 1: {
					usePrivilege_Qt();

					break;
				}
				case 2: {
					tokenSelection_Qt();

					end_choice = 1;
					break;
				}
				case 3: {
					cardPurchase_Qt();

					end_choice = 1;
					break;
				}
				default: {
					break;
				}
				}
				break;
			}
			case (2): { // only possible to fill the board
				switch (tmp) {
				case 1: { // fill board
					fillingBoard();

					break;
				}
				case 2: {
					tokenSelection_Qt();

					end_choice = 1;
					break;
				}
				case 3: {
					cardPurchase_Qt();

					end_choice = 1;
					break;
				}
				default: {
					break;
				}
				}
				break;
			}
			case (3): { // two optional choices possible
				switch (tmp) {
				case 1: {
					usePrivilege_Qt();

					break;
				}
				case 2: { // fill board
					fillingBoard();

					break;
				}
				case 3: {
					tokenSelection_Qt();

					end_choice = 1;
					break;
				}
				case 4: {
					cardPurchase_Qt();

					end_choice = 1;
					break;
				}

				default: {
					break;
				}
				}
				break;
			}
			default: {
				break;
			}
			}
			MainWindow::getMainWindow().updateBoard();
			MainWindow::getMainWindow().updateDraws();
			MainWindow::getMainWindow().updateScores();
			MainWindow::getMainWindow().updatePrivileges();
			MainWindow::getMainWindow().update();
		} catch (SplendorException &e) {
			MainWindow::getMainWindow().triggerInfo(e.getInfo());
			cout << e.getInfo() << "\n";
		}
	}
}

void Player::usePrivilege() {
	if (privilege_number <= 0)
		throw SplendorException("You do not have a privilege!");
	unsigned int index;
	cout << "Which token do you want to draw? " << endl;
	cout << "index : ";
	cin >> index;
	Game::getGame().getPlayerRound().drawToken(index);
	withdrawPrivilege();
}

void Player::usePrivilege_Qt() {
	if (privilege_number <= 0)
		throw SplendorException("You do not have a privilege!");
	int index = -1;

	MainWindow::getMainWindow().triggerInfo("Please draw a token");
	// Activate the tokens
	MainWindow::getMainWindow().activateTokens();

	while (index == -1 or
	       (index != -1 and
	        Board::getBoard().getBoardCaseByIndex(index)->getColor() ==
	            Color::gold)) {
		MainWindow::getMainWindow().getTokenWaitLoop()->exec();
		index = MainWindow::getMainWindow().getTokenIndexOnClick();
	}

	Game::getGame().getPlayerRound().drawToken(index);
	withdrawPrivilege();
}

void Player::tokenSelection() {
	bool nb_ok = 0;
	bool choice_ok = 0;
	if (Board::getBoard().onlyGold() and
	    Game::getGame().getPlayerRound().getReservedCardNumber() >= 3)
		throw SplendorException(
		    "Cannot draw: only gold tokens remain and you cannot "
		    "reserve any more cards!");
	while (!choice_ok) {
		try {
			std::vector<int> tmp_tab(0);
			optional<Position> pos = nullopt;
			int gold_number = 0;
			int perl_number = 0;
			string validation;
			while (validation != "Y") {
				while (tmp_tab.size() < 3) {
					unsigned int index = 0;
					cout << "Please enter the index of token "
					     << tmp_tab.size() << " that you want to take ";
					if (tmp_tab.size() >
					    0) { // added possibility to stop
						cout << "-1 to stop token selection";
					}
					cout << " :" << endl;
					cout << "choices :";
					cin >> index;
					if (index == -1) {
						break;
					}
					if (Board::getBoard().getBoardCaseByIndex(index) ==
					    nullptr) { // the number of cells on the board
						           // corresponds to the number of tokens in the
						           // game
						gold_number = 0;
						perl_number = 0;
						tmp_tab.clear();
						throw SplendorException(
						    "There is no token at this index!\n");
					}
					if (index > Token::getMaxTokenNumber()) { // the number of
						                                      // cells on the
						// board corresponds to
						// the number of tokens in the
						// game
						gold_number = 0;
						perl_number = 0;
						tmp_tab.clear();
						throw SplendorException(
						    "There are only " +
						    std::to_string(Token::getMaxTokenNumber()) +
						    " places on the board\n");
					}
					if (Board::getBoard()
					        .getBoardCaseByIndex(index)
					        ->getColor() == Color::gold) {
						gold_number++;
					}
					if (Board::getBoard()
					        .getBoardCaseByIndex(index)
					        ->getColor() == Color::perl) {
						perl_number++;
					}
					if ((gold_number == 1) &&
					    (Game::getGame()
					         .getPlayerRound()
					         .getReservedCardNumber() >= 3)) {
						gold_number = 0;
						perl_number = 0;
						tmp_tab.clear();
						throw SplendorException(
						    "You are not allowed to reserve an "
						    "additional card!");
					}
					if (gold_number == 1 and tmp_tab.size() > 0) {
						gold_number = 0;
						perl_number = 0;
						tmp_tab.clear();
						throw SplendorException(
						    "Warning, you can only take a gold token "
						    "by itself!");
					}
					tmp_tab.push_back(index);
				}
				cout << "Do you validate your selection? [Y/N] ";
				cin >> validation;
				if (validation != "Y") { // restart token selection
					tmp_tab.clear();
					gold_number = 0;
					perl_number = 0;
					cout << "\n You will restart token selection: "
					     << endl;
					cout << "Board" << endl;
					//                    for(int test = 0;
					//                    test<tmp_tab.size();test++){
					//                        cout<<tmp_tab[test]<<" - ";
					//                    }
				}
			}

			// sort the vector by selection
			int min = 0;
			for (int j = 0; j < tmp_tab.size() - 1; j++) {
				min = j;
				for (int k = j + 1; k < tmp_tab.size(); k++) {
					if (tmp_tab[k] < tmp_tab[min]) {
						min = k;
					}
				}
				if (min != j) {
					int tmp2 = tmp_tab[min];
					tmp_tab[min] = tmp_tab[j];
					tmp_tab[j] = tmp2;
				}
			}
			// sorted vector
			// cout<<"sorted vector\n";

			if (tmp_tab.size() ==
			    2) { // alignment check for 2 tokens
				// cout<<"check for 2 tokens\n";
				const Token *tok1 =
				    Board::getBoard().getBoardCaseByIndex(tmp_tab[0]);
				optional<Position> pos1 =
				    Board::getBoard().tokensAreASide(tmp_tab[1], tok1);
				if (pos1 == nullopt) {
					throw SplendorException("Tokens not aligned\n");
				}
			}
			if (tmp_tab.size() ==
			    3) { // alignment check for 3 tokens
				// cout<<"check for 3 tokens\n";
				const Token *tok1 =
				    Board::getBoard().getBoardCaseByIndex(tmp_tab[1]);
				optional<Position> pos1 =
				    Board::getBoard().tokensAreASide(tmp_tab[0], tok1);

				const Token *tok2 =
				    Board::getBoard().getBoardCaseByIndex(tmp_tab[2]);
				optional<Position> pos2 =
				    Board::getBoard().tokensAreASide(tmp_tab[1], tok2);

				if ((pos1 != pos2) || (pos1 == nullopt) || (pos2 == nullopt)) {
					throw SplendorException("Tokens not aligned\n");
				}
			}

			// we have verified the token alignment
			if (perl_number ==
			    2) { // opponent gets a privilege if
				     // we draw 2 perl tokens at once
				Game::getGame().getOpponent().obtainPrivilege();
			}

			if (tmp_tab.size() ==
			    3) { // opponent gets a privilege if
				     // all 3 tokens are the same color
				if ((Board::getBoard()
				         .getBoardCaseByIndex(tmp_tab[0])
				         ->getColor() == Board::getBoard()
				                             .getBoardCaseByIndex(tmp_tab[1])
				                             ->getColor()) &&
				    (Board::getBoard()
				         .getBoardCaseByIndex(tmp_tab[1])
				         ->getColor() == Board::getBoard()
				                             .getBoardCaseByIndex(tmp_tab[2])
				                             ->getColor())) {
					Game::getGame().getOpponent().obtainPrivilege();
				}
			}
			for (int i = 0; i < tmp_tab.size();
			     i++) { // token acquisition by the player
				Game::getGame().getCurrentPlayer().drawToken(tmp_tab[i]);
			}
			if (gold_number == 1) {
				cardReservation();
			}
			choice_ok = 1;
		} catch (SplendorException &e) {
			cout << e.getInfo() << "\n";
		}
	}
}

void Player::tokenSelection_Qt() {
	MainWindow::getMainWindow().activateTokens();

	// qDebug() << "SELECTION TOKENS QT";
	bool nb_ok = 0;
	bool choice_ok = 0;
	if (Board::getBoard().onlyGold() and
	    Game::getGame().getPlayerRound().getReservedCardNumber() >= 3) {
		throw SplendorException(
		    "Cannot draw: only gold tokens remain and you cannot "
		    "reserve any more cards!");
	}
	while (!choice_ok) {
		try {
			qDebug() << "Try selection";
			std::vector<int> tmp_tab(0);
			optional<Position> pos = nullopt;
			int gold_number = 0;
			int perl_number = 0;
			string validation;
			char choice_valid = 'a';
			while (choice_valid != 'Y') {
				MainWindow::getMainWindow().activateTokens();
				qDebug() << "While choice_valid";
				while (tmp_tab.size() < 3) {
					qDebug() << "While size()";
					int index = 0;

					MainWindow::getMainWindow().getTokenWaitLoop()->exec();

					index = MainWindow::getMainWindow().getTokenIndexOnClick();
					qDebug() << index;
					if (index == -1)
						continue;

					if (Board::getBoard().getBoardCaseByIndex(index) ==
					    nullptr) { // the number of cells on the board
						           // corresponds to the number of tokens in the
						           // game
						gold_number = 0;
						perl_number = 0;
						tmp_tab.clear();
						throw SplendorException(
						    "There is no token at this index!\n");
					}
					if (index > Token::getMaxTokenNumber()) { // the number of
						                                      // cells on the
						// board corresponds to
						// the number of tokens in the
						// game
						gold_number = 0;
						perl_number = 0;
						tmp_tab.clear();
						throw SplendorException(
						    "There are only " +
						    std::to_string(Token::getMaxTokenNumber()) +
						    " places on the board\n");
					}
					if (Board::getBoard()
					        .getBoardCaseByIndex(index)
					        ->getColor() == Color::gold) {
						gold_number++;
					}
					if (Board::getBoard()
					        .getBoardCaseByIndex(index)
					        ->getColor() == Color::perl) {
						perl_number++;
					}
					if ((gold_number == 1) &&
					    (Game::getGame()
					         .getPlayerRound()
					         .getReservedCardNumber() >= 3)) {
						gold_number = 0;
						perl_number = 0;
						tmp_tab.clear();
						throw SplendorException(
						    "You are not allowed to reserve an "
						    "additional card!");
					}
					if (gold_number == 1 and tmp_tab.size() > 0) {
						gold_number = 0;
						perl_number = 0;
						tmp_tab.clear();
						throw SplendorException(
						    "Warning, you can only take a gold token "
						    "by itself!");
					}
					tmp_tab.push_back(index);

					if (tmp_tab.size() < 3) {
						MainWindow::getMainWindow().triggerYesNo(
						    &choice_valid, "Select another token ");

						if (choice_valid == 'Y')
							qDebug() << "Yes choice";
						else
							qDebug() << "No choice";

						if (choice_valid == 'N') {
							break;
						}
					}
				}

				MainWindow::getMainWindow().triggerYesNo(
				    &choice_valid, "Do you validate your selection?");
				/*
				cout<<"Do you validate your selection? [Y/N] ";
				cin>>validation;
				*/
				if (choice_valid !=
				    'Y') { // restart token selection
					tmp_tab.clear();
					gold_number = 0;
					perl_number = 0;
					MainWindow::getMainWindow().triggerInfo(
					    "You will restart token selection.");

					// cout<<"\n You will restart the token selection:
					// "<<endl; cout<<"Array"<<endl;
					//                    for(int test = 0;
					//                    test<tmp_tab.size();test++){
					//                        cout<<tmp_tab[test]<<" - ";
					//                    }
				}
			}

			// sort the vector by selection
			int min = 0;
			for (int j = 0; j < tmp_tab.size() - 1; j++) {
				min = j;
				for (int k = j + 1; k < tmp_tab.size(); k++) {
					if (tmp_tab[k] < tmp_tab[min]) {
						min = k;
					}
				}
				if (min != j) {
					int tmp2 = tmp_tab[min];
					tmp_tab[min] = tmp_tab[j];
					tmp_tab[j] = tmp2;
				}
			}

			// ### NON-ALIGNMENT HANDLING
			if (tmp_tab.size() ==
			    2) { // alignment check for 2 tokens
				// cout<<"check for 2 tokens\n";
				const Token *tok1 =
				    Board::getBoard().getBoardCaseByIndex(tmp_tab[0]);
				optional<Position> pos1 =
				    Board::getBoard().tokensAreASide(tmp_tab[1], tok1);
				if (pos1 == nullopt) {
					throw SplendorException("Tokens not aligned\n");
				}
			}
			if (tmp_tab.size() ==
			    3) { // alignment check for 3 tokens
				// cout<<"check for 3 tokens\n";
				const Token *tok1 =
				    Board::getBoard().getBoardCaseByIndex(tmp_tab[1]);
				optional<Position> pos1 =
				    Board::getBoard().tokensAreASide(tmp_tab[0], tok1);

				const Token *tok2 =
				    Board::getBoard().getBoardCaseByIndex(tmp_tab[2]);
				optional<Position> pos2 =
				    Board::getBoard().tokensAreASide(tmp_tab[1], tok2);

				if ((pos1 != pos2) || (pos1 == nullopt) || (pos2 == nullopt)) {
					tmp_tab.clear();
					throw SplendorException("Tokens not aligned\n");
				}
			}

			// we have verified the token alignment
			if (perl_number ==
			    2) { // opponent gets a privilege if
				     // we draw 2 perl tokens at once
				Game::getGame().getOpponent().obtainPrivilege();
			}

			if (tmp_tab.size() ==
			    3) { // opponent gets a privilege if
				     // all 3 tokens are the same color
				if ((Board::getBoard()
				         .getBoardCaseByIndex(tmp_tab[0])
				         ->getColor() == Board::getBoard()
				                             .getBoardCaseByIndex(tmp_tab[1])
				                             ->getColor()) &&
				    (Board::getBoard()
				         .getBoardCaseByIndex(tmp_tab[1])
				         ->getColor() == Board::getBoard()
				                             .getBoardCaseByIndex(tmp_tab[2])
				                             ->getColor())) {
					Game::getGame().getOpponent().obtainPrivilege();
				}
			}
			for (int i = 0; i < tmp_tab.size();
			     i++) { // token acquisition by the player
				Game::getGame().getCurrentPlayer().drawToken(tmp_tab[i]);
			}

			if (gold_number == 1) {
				MainWindow::getMainWindow().deactivateButtons();
				cardReservation_Qt();
			}
			choice_ok = 1;
		} catch (SplendorException &e) {
			MainWindow::getMainWindow().triggerInfo(e.getInfo() +
			                                        "Please try again: ");
			cout << e.getInfo() << "\n";
		}
	}
}

void Player::cardReservation() {
	cout << "\n\nDraw1 :" << endl;
	cout << *Game::getGame().getFirstDraw() << endl;
	cout << "\nDraw2 :" << endl;
	cout << *Game::getGame().getSecondDraw() << endl;
	cout << "\nDraw3 :" << endl;
	cout << *Game::getGame().getThirdDraw() << endl;
	string drawOrDeck;
	cout << "\nDo you want to reserve a card from a draw or the first "
	        "card of a deck? [T/P]"
	     << endl;
	cin >> drawOrDeck;
	if (drawOrDeck != "P") {
		// The player chooses to reserve the card from a draw
		int choices = 0;
		while (choices != 1 && choices != 2 && choices != 3) {
			cout << "From which draw do you want to reserve a card?" << endl;
			cout << "choices";
			cin >> choices;
		}
		switch (choices) {
		case 1: {
			unsigned int index = 0;
			cout << "Please enter the index of the card you want "
			        "to remove!"
			     << endl;
			cout << "choices : ";
			cin >> index;
			reserveCard(Game::getGame().getFirstDraw(), index);
			break;
		}
		case 2: {
			unsigned int index = 0;
			cout << "Please enter the index of the card you want "
			        "to remove!"
			     << endl;
			cout << "choices : ";
			cin >> index;
			reserveCard(Game::getGame().getSecondDraw(), index);
			break;
		}
		case 3: {
			unsigned int index = 0;
			cout << "Please enter the index of the card you want "
			        "to remove!"
			     << endl;
			cout << "choices : ";
			cin >> index;
			reserveCard(Game::getGame().getThirdDraw(), index);
			break;
		}
		}
	} else {
		int choices = 0;
		cout << "On which deck do you want to reserve the card? " << endl;
		cout << "choices: ";
		cin >> choices;
		while (choices != 1 && choices != 2 && choices != 3) {
			cout << "On which deck do you want to reserve a card?" << endl;
			cout << "choices";
			cin >> choices;
		}
		reserveCard(Game::getGame().getDeck(choices));
	}
}

void Player::cardReservation_Qt() {
	MainWindow::getMainWindow().activateForReserve();
	string drawOrDeck;

	MainWindow::getMainWindow().activateForBuy();
	MainWindow::getMainWindow().getCardWaitLoop()->exec();

	// Click card and get the ref of the card and index in draw or in the
	// reserved cards
	Qt_card *last_card_clicked =
	    MainWindow::getMainWindow().getLastCardClicked();

	// Card in the deck
	if (last_card_clicked->getIndex() < 0) {
		Game::getGame().getCurrentPlayer().reserveCard(
		    Game::getGame().getDeck(-1 * last_card_clicked->getIndex()));
	}

	// Card in draw
	else {
		const JewelryCard *c =
		    dynamic_cast<const JewelryCard *>(last_card_clicked->getCard());
		int draw_level = c->getLevel();
		Draw *draw = Game::getGame().getDrawById(draw_level);
		Game::getGame().getCurrentPlayer().reserveCard(
		    draw, last_card_clicked->getIndex());
	}
}

void Player::applyCapacity(const JewelryCard &card, StrategyPlayer &opponent) {
	if (card.getCapacity().has_value()) {
		std::optional<Capacity> capa = card.getCapacity();
		if (capa == Capacity::steal_opponent_pawn) {
			cout << "Capacity usage: you can take a gem or "
			        "perl token "
			        "from your opponent.\n";

			if ((opponent.getToken().empty()) &&
			    (!opponent.onlyGoldInTokens())) {
				cout << "Too bad, your opponent does not have a gem or "
				        "perl "
				        "token!"
				     << endl;
			} else {
				vector<const Token *> opponent_tokens = opponent.getToken();
				cout << "Here are your opponent's tokens: " << endl;
				int i = 0;
				for (auto tok : opponent_tokens) {
					cout << "Index: " << i++ << ", " << *tok << endl;
				}
				int choice = -1;
				do {
					if (choice != -1) {
						cout << "You cannot take a gold token!"
						     << endl;
					}
					cout << "Which token do you want to steal?" << endl;
					cout << "Choice: ";
					cin >> choice;
				} while (opponent_tokens[choice]->getColor() == Color::gold);
				tokens.push_back(opponent_tokens[choice]);
				token_number++;
				opponent_tokens.erase(opponent_tokens.begin() + choice);
				opponent.setNbTokens(opponent.getTokenNumber() - 1);
			}
		} else if (capa == Capacity::take_priviledge) {
			Game::getGame().getCurrentPlayer().obtainPrivilege();
		} else if (capa == Capacity::take_on_board) {
			cout << "Capacity usage: you can take a token of the "
			        "bonus "
			        "color of the card\n";
			const optional<enum colorBonus> &color = card.getBonus();
			if (Board::getBoard().colorsOnBoard(color)) {
				bool choice_ok = 0;
				unsigned int index = 0;
				while (!choice_ok) {
					try {
						cout << "Please enter the index of the token you "
						        "want to "
						        "take\n ";
						cout << "choices :" << endl;
						cin >> index;
						if (Board::getBoard().getBoardCaseByIndex(index) ==
						    nullptr) {
							index = 0;
							throw SplendorException(
							    "There is no token at this index!\n");
						}
						if (index < 0 or
						    index >= Token::getMaxTokenNumber()) { // the number
							                                       // of spaces
							// on the board
							// matches the
							// number of tokens
							// in the game
							index = 0;
							throw SplendorException(
							    "There are only " +
							    std::to_string(Token::getMaxTokenNumber()) +
							    " spaces on the board\n");
						}
						string s = "Bonus ";
						if (s + toString(Board::getBoard()
						                     .getBoardCaseByIndex(index)
						                     ->getColor()) !=
						    toString(color)) {
							index = 0;
							throw SplendorException(
							    "you must choose a token of the bonus "
							    "color!\n");
						} else {
							Game::getGame().getCurrentPlayer().drawToken(index);
							choice_ok = 1;
						}
					} catch (SplendorException &e) {
						cout << e.getInfo() << "\n";
					}
				}
			}
		} else { // if the capacity is to replay
			Game::getGame().nextRound(1);
		}
	}
	if (card.getBonus() == colorBonus::joker) {
		cout << "Capacity usage: you can transform the joker into "
		        "a "
		        "color bonus by associating it with "
		        "one of your cards that has at least one bonus.\n";
		int bonus_white = calculateBonus(colorBonus::white);
		int bonus_blue = calculateBonus(colorBonus::blue);
		int bonus_red = calculateBonus(colorBonus::red);
		int bonus_green = calculateBonus(colorBonus::green);
		int bonus_black = calculateBonus(colorBonus::black);

		bool choice_verified = false;
		try {
			while (!choice_verified) {
				cout << "Make your choice:" << endl;
				int option = 0;
				if (bonus_white > 0) {
					cout << "Bonus white [1]" << endl;
					option++;
				}
				if (bonus_blue > 0) {
					cout << "Bonus blue [2]" << endl;
					option++;
				}
				if (bonus_red > 0) {
					cout << "Bonus red [3]" << endl;
					option++;
				}
				if (bonus_green > 0) {
					cout << "Bonus green [4]" << endl;
					option++;
				}
				if (bonus_black > 0) {
					cout << "Bonus black [5]" << endl;
					option++;
				}
				// Check if no option is available
				if (option == 0) {
					choice_verified = true;
					throw SplendorException("you do not own any card "
					                        "with a bonus.. Capacity"
					                        " has no effect\n");
				} else {
					int choices;
					cin >> choices;
					colorBonus b;
					switch (choices) {
					case 1:
						b = colorBonus::white;
						card.changeBonusColor(b);
						choice_verified = true;
						break;
					case 2:
						b = colorBonus::blue;
						card.changeBonusColor(b);
						choice_verified = true;
						break;
					case 3:
						b = colorBonus::red;
						card.changeBonusColor(b);
						choice_verified = true;
						break;
					case 4:
						b = colorBonus::green;
						card.changeBonusColor(b);
						choice_verified = true;
						break;
					case 5:
						b = colorBonus::black;
						card.changeBonusColor(b);
						;
						choice_verified = true;
						break;
					default:
						cout << "Invalid choice, please try again.\n";
						break;
					}
				}
			}
		} catch (SplendorException &e) {
			cout << e.getInfo() << "\n";
		}
	}
}

// Qt overload
void Player::applyCapacity_Qt(const JewelryCard &card,
                              StrategyPlayer &opponent) {
	MainWindow::getMainWindow().deactivateButtons();
	if (card.getCapacity().has_value()) {
		std::optional<Capacity> capa = card.getCapacity();
		if (capa == Capacity::steal_opponent_pawn) {
			if (Game::getGame().getOpponent().getTokenNumber() != 0) {
				MainWindow::getMainWindow().deactivateButtons();
				MainWindow::getMainWindow().setStealingToken(true);

				MainWindow::getMainWindow().triggerInfo(
				    "Capacity usage: you must take a gem or "
				    "perl token "
				    "from your opponent");
				MainWindow::getMainWindow().getTokenWaitLoop()->exec();

				int index = MainWindow::getMainWindow()
				                .getTokenIndexOnClick(); // index to retrieve
				                                         // via qt

				qDebug() << index;

				// Add to the player and remove from the opponent
				vector<const Token *> &opponent_tokens = opponent.getToken();
				tokens.push_back(opponent_tokens[index]);
				token_number++;
				opponent_tokens.erase(opponent_tokens.begin() + index);
				opponent.setNbTokens(opponent.getTokenNumber() - 1);
				MainWindow::getMainWindow().setStealingToken(false);
			}

		} else if (capa == Capacity::take_priviledge) {
			// ok no need to change for Qt
			MainWindow::getMainWindow().triggerInfo(
			    "You are going to obtain a privilege");
			Game::getGame().getCurrentPlayer().obtainPrivilege();
		} else if (capa == Capacity::take_on_board) {
			const optional<colorBonus> &color = card.getBonus();
			if (Board::getBoard().colorsOnBoard(color)) {
				MainWindow::getMainWindow().triggerInfo(
				    "Capacity usage: you can take a " +
				    toString(card.getBonus()) + " token on the board");

				MainWindow::getMainWindow().activateTokenColor(
				    colorBonusToColor(color));

				MainWindow::getMainWindow().getTokenWaitLoop()->exec();
				int index = MainWindow::getMainWindow()
				                .getTokenIndexOnClick(); // index to retrieve
				                                         // via qt

				Game::getGame().getCurrentPlayer().drawToken(index);
			}

		} else { // The only possible capacity is to replay
			// Nothing to change for Qt
			MainWindow::getMainWindow().triggerInfo("You are going to replay");
			Game::getGame().nextRound(1);
		}
	}
	if (card.getBonus() == colorBonus::joker) {
		colorBonus b;
		MainWindow::getMainWindow().triggercolorJoker(&b);
		card.changeBonusColor(b);
	}
}

void Player::applyRoyalCapacity(const RoyalCard &card,
                                StrategyPlayer &opponent) {
	if (card.getCapacity().has_value()) {
		std::optional<Capacity> capa = card.getCapacity();
		if (capa == Capacity::steal_opponent_pawn) {
			cout << "Capacity usage: you can take a gem or "
			        "perl token "
			        "from your opponent.\n";

			if (opponent.getToken().empty()) {
				cout << "Too bad, your opponent does not have a gem or "
				        "perl "
				        "token!"
				     << endl;
			} else {
				vector<const Token *> opponent_tokens = opponent.getToken();
				cout << "Here are your opponent's tokens: " << endl;
				int i = 0;
				for (auto tok : opponent_tokens) {
					cout << "Index: " << i++ << ", " << *tok << endl;
				}
				int choice = -1;
				do {
					if (choice != -1) {
						cout << "You cannot take a gold token!"
						     << endl;
					}
					cout << "Which token do you want to steal?" << endl;
					cout << "Choice: ";
					cin >> choice;
				} while (opponent_tokens[choice]->getColor() == Color::gold);
				tokens.push_back(opponent_tokens[choice]);
				token_number++;
				opponent_tokens.erase(opponent_tokens.begin() + choice);
				opponent.setNbTokens(opponent.getTokenNumber() - 1);
			}
		} else if (capa == Capacity::take_priviledge) {
			Game::getGame().getCurrentPlayer().obtainPrivilege();
			cout << "Card capacity: You have obtained a privilege!"
			     << endl;
		} else {
			Game::getGame().nextRound(1);
			cout << "Card capacity: Replay! You will restart"
			     << endl;
		}
	}
}

// Qt overload
void Player::applyRoyalCapacity_Qt(const RoyalCard &card,
                                   StrategyPlayer &opponent) {
	if (card.getCapacity().has_value()) {
		std::optional<Capacity> capa = card.getCapacity();
		if (capa == Capacity::steal_opponent_pawn) {
			if (Game::getGame().getOpponent().getTokenNumber() != 0) {
				MainWindow::getMainWindow().deactivateButtons();
				MainWindow::getMainWindow().setStealingToken(true);

				MainWindow::getMainWindow().triggerInfo(
				    "Capacity usage: you must take a gem or "
				    "perl token "
				    "from your opponent");
				MainWindow::getMainWindow().getTokenWaitLoop()->exec();

				int index = MainWindow::getMainWindow()
				                .getTokenIndexOnClick(); // index to retrieve
				                                         // via qt

				qDebug() << index;

				// Add to the player and remove from the opponent
				vector<const Token *> &opponent_tokens = opponent.getToken();
				tokens.push_back(opponent_tokens[index]);
				token_number++;
				opponent_tokens.erase(opponent_tokens.begin() + index);
				opponent.setNbTokens(opponent.getTokenNumber() - 1);
				MainWindow::getMainWindow().setStealingToken(false);
			}
		} else if (capa == Capacity::take_priviledge) {
			// Nothing to change in Qt
			MainWindow::getMainWindow().triggerInfo(
			    "You obtain a privilege");
			Game::getGame().getCurrentPlayer().obtainPrivilege();
			cout << "Card capacity: You have obtained a privilege!"
			     << endl;
		} else {
			MainWindow::getMainWindow().triggerInfo("You are going to replay");
			Game::getGame().nextRound(1);
			cout << "Card capacity: Replay! You will restart"
			     << endl;
		}
	}
}

void Player::cardPurchase() {
	unsigned int choice = -1;
	if (Game::getGame().getCurrentPlayer().getReservedCardNumber() != 0) {

		while (choice != 1 && choice != 0) {
			cout << "Do you want to buy a card you previously reserved? "
			        "1 for yes "
			        "/ 0 for no"
			     << endl;
			cout << "choices";
			cin >> choice;
		}
		if (choice == 1) {
			// then we must show the cards they can buy (those already reserved)
			vector<const JewelryCard *> &reserved =
			    Game::getGame().getCurrentPlayer().getReservedCards();
			for (int i = 0; i < reserved.size(); ++i) {
				cout << "index : " << i << " " << *reserved[i] << endl;
			}
			unsigned int index;
			cout << "please enter the index of the chosen card: ";
			cin >> index;
			Game::getGame().getCurrentPlayer().buyReservedCard(index);
		} else {
			cout << "\n\nDraw1 :" << endl;
			cout << *Game::getGame().getFirstDraw() << endl;
			cout << "\nDraw2 :" << endl;
			cout << *Game::getGame().getSecondDraw() << endl;
			cout << "\nDraw3 :" << endl;
			cout << *Game::getGame().getThirdDraw() << endl;
			int choices = 0;
			while (choices != 1 && choices != 2 && choices != 3) {
				cout << "From which draw do you want to buy a card?" << endl;
				cout << "choices";
				cin >> choices;
			}
			switch (choices) {
			case 1: {
				unsigned int index = 0;
				cout << "Please enter the index of the card you "
				        "want "
				        "to buy: "
				     << endl;
				cout << "choices : ";
				cin >> index;
				Game::getGame().getCurrentPlayer().buyCard(
				    Game::getGame().getFirstDraw(), index);
				break;
			}
			case 2: {
				unsigned int index = 0;
				cout << "Please enter the index of the card you "
				        "want "
				        "to buy: "
				     << endl;
				cout << "choices : ";
				cin >> index;
				Game::getGame().getCurrentPlayer().buyCard(
				    Game::getGame().getSecondDraw(), index);
				break;
			}
			case 3: {
				unsigned int index = 0;
				cout << "Please enter the index of the card you "
				        "want "
				        "to buy: "
				     << endl;
				cout << "choices : ";
				cin >> index;
				Game::getGame().getCurrentPlayer().buyCard(
				    Game::getGame().getThirdDraw(), index);
				break;
			}
			}
		}
	} else {
		cout << "\n\nDraw1 :" << endl;
		cout << *Game::getGame().getFirstDraw() << endl;
		cout << "\nDraw2 :" << endl;
		cout << *Game::getGame().getSecondDraw() << endl;
		cout << "\nDraw3 :" << endl;
		cout << *Game::getGame().getThirdDraw() << endl;
		int choices = 0;
		while (choices != 1 && choices != 2 && choices != 3) {
			cout << "From which draw do you want to buy a card?" << endl;
			cout << "choices";
			cin >> choices;
		}
		switch (choices) {
		case 1: {
			unsigned int index = 0;
			cout << "Please enter the index of the card you want "
			        "to buy: "
			     << endl;
			cout << "choices : ";
			cin >> index;
			Game::getGame().getCurrentPlayer().buyCard(
			    Game::getGame().getFirstDraw(), index);
			break;
		}
		case 2: {
			unsigned int index = 0;
			cout << "Please enter the index of the card you want "
			        "to buy: "
			     << endl;
			cout << "choices : ";
			cin >> index;
			Game::getGame().getCurrentPlayer().buyCard(
			    Game::getGame().getSecondDraw(), index);
			break;
		}
		case 3: {
			unsigned int index = 0;
			cout << "Please enter the index of the card you want "
			        "to buy: "
			     << endl;
			cout << "choices : ";
			cin >> index;
			Game::getGame().getCurrentPlayer().buyCard(
			    Game::getGame().getFirstDraw(), index);
			break;
		}
		}
	}
}

// Qt overload
void Player::cardPurchase_Qt() {
	MainWindow::getMainWindow().setBuyingCard(true);
	// qDebug() << "Card qt";
	MainWindow::getMainWindow().activateForBuy();
	MainWindow::getMainWindow().getCardWaitLoop()->exec();

	Qt_card *last_card_clicked =
	    MainWindow::getMainWindow().getLastCardClicked();
	qDebug() << last_card_clicked->getIndex()
	         << last_card_clicked->getReserved()
	         << last_card_clicked->getCard()->getVisual();

	// Reserved cards
	if (last_card_clicked->getReserved() == true) {
		qDebug() << "CARD EFFECTIVELY RESERVED";
		Game::getGame().getCurrentPlayer().buyReservedCard_Qt(
		    last_card_clicked->getIndex());
	}

	// Card in draw
	else {
		qDebug() << "CARD NOT EFFECTIVELY RESERVED";
		const JewelryCard *c =
		    dynamic_cast<const JewelryCard *>(last_card_clicked->getCard());
		int draw_level = c->getLevel();
		Draw *draw = Game::getGame().getDrawById(draw_level);
		Game::getGame().getCurrentPlayer().buyCard_Qt(
		    draw, last_card_clicked->getIndex());
	}
	MainWindow::getMainWindow().setBuyingCard(false);
	last_card_clicked->setReserved(false);
}

void Player::buyCard(Draw *t, const int index) {

	// the card to remove is the i-th from draw t

	const JewelryCard &card = t->getCardWithoutDeletion(index);
	if (card.getBonus() == colorBonus::joker and
	    jokerCardEligibility() == false) {
		throw SplendorException("You are not allowed to buy a card "
		                        "with a joker bonus because "
		                        "you cannot assign the bonus!");
	}

	// here calculating bonus removes the bonus from the total card cost
	// of already owned cards.
	int white_cost =
	    positiveOrNull(card.getCostWhite() - calculateBonus(colorBonus::white));
	int blue_cost =
	    positiveOrNull(card.getCostBlue() - calculateBonus(colorBonus::blue));
	int red_cost =
	    positiveOrNull(card.getCostRed() - calculateBonus(colorBonus::red));
	int green_cost =
	    positiveOrNull(card.getCostGreen() - calculateBonus(colorBonus::green));
	int black_cost =
	    positiveOrNull(card.getCostBlack() - calculateBonus(colorBonus::black));
	int perl_cost = card.getCostPerl();

	// Check if the player wants to use gold tokens (if they have any)
	// And reduce the cost accordingly
	int nb_gold = 0;
	if (TokenAmount(Color::gold) > 0) {
		std::cout << "Do you want to use gold token(s)? [Y/N]"
		          << std::endl;
		std::string choices;
		std::cin >> choices;
		int nb;
		if (choices == "Y") {
			while (TokenAmount(Color::gold) > nb_gold) {
				// choose the color
				std::cout << "Color to replace? Stop to stop using "
				             "gold tokens:"
				          << std::endl;
				std::cin >> choices;
				if (choices == "stop" || "Stop") {
					break;
				}
				do {
					// choose the number of gold tokens used for the color in
					// question
					std::cout << "How many?" << std::endl;
					std::cin >> nb;
				} while (nb > TokenAmount(Color::gold));
				try {
					if (nb > TokenAmount(Color::gold) - nb_gold)
						throw SplendorException(
						    "You don't have enough gold tokens to "
						    "spend that many!");
					if ((choices == "white" || "White") && (nb > white_cost))
						throw SplendorException(
						    "The white cost is less than the number of gold "
						    "tokens you "
						    "want to use as joker!");
					if ((choices == "blue" || "Blue") && (nb > blue_cost))
						throw SplendorException(
						    "The blue cost is less than the number of gold tokens "
						    "you "
						    "want to use as joker!");
					if ((choices == "red" || "Red") && (nb > red_cost))
						throw SplendorException(
						    "The red cost is less than the number of gold "
						    "tokens you "
						    "want to use as joker!");
					if ((choices == "green" || "Green") && (nb > green_cost))
						throw SplendorException(
						    "The green cost is less than the number of gold "
						    "tokens "
						    "you "
						    "want to use as joker!");
					if ((choices == "black" || "Black") && (nb > black_cost))
						throw SplendorException(
						    "The black cost is less than the number of gold "
						    "tokens "
						    "you "
						    "want to use as joker!");
					if ((choices == "perl" || "Pearl") && (nb > white_cost))
						throw SplendorException(
						    "The perl cost is less than the number of gold "
						    "tokens you "
						    "want to use as joker!");
					if (choices != "white" || "White" || "blue" || "Blue" ||
					    "red" || "Red" || "green" || "Green" || "black" ||
					    "Black" || "perl" || "Pearl")
						throw SplendorException("Unknown token color!");

					// If all checks passed, we can use the gold
					// tokens
					nb_gold += nb;
					if (choices == "white" || "White")
						white_cost = positiveOrNull(white_cost - nb);
					if (choices == "blue" || "Blue")
						blue_cost = positiveOrNull(blue_cost - nb);
					if (choices == "red" || "Red")
						red_cost = positiveOrNull(red_cost - nb);
					if (choices == "green" || "Green")
						green_cost = positiveOrNull(green_cost - nb);
					if (choices == "black" || "Black")
						black_cost = positiveOrNull(black_cost - nb);
					if (choices == "perl" || "Pearl")
						perl_cost = positiveOrNull(perl_cost - nb);
				} catch (SplendorException e) {
					cout << e.getInfo() << endl;
				}
			}
		}
	}
	// Check if enough tokens
	int eligible_purchase = 0;

	// check if we have enough tokens to buy
	if (TokenAmount(Color::white) >= white_cost &&
	    TokenAmount(Color::blue) >= blue_cost &&
	    TokenAmount(Color::red) >= red_cost &&
	    TokenAmount(Color::green) >= green_cost &&
	    TokenAmount(Color::black) >= black_cost &&
	    TokenAmount(Color::perl) >= perl_cost) {
		eligible_purchase = 1;
	}

	if (eligible_purchase == 0)
		throw SplendorException("Not enough tokens to buy the card!");

	// Remove used tokens and put them back in the bag
	withdrawTokens(Color::white, white_cost);
	withdrawTokens(Color::blue, blue_cost);
	withdrawTokens(Color::red, red_cost);
	withdrawTokens(Color::green, green_cost);
	withdrawTokens(Color::black, black_cost);
	withdrawTokens(Color::perl, perl_cost);
	withdrawTokens(Color::gold, nb_gold);

	// Put the card in the player's hand and remove it from the draw
	bought_jewelry_cards.push_back(&(t->getCard(index)));
	try {
		t->fill();
	} catch (SplendorException &e) {
	}

	// Add the crown count
	crown_number += card.getNbCrown();
	points_number += card.getPrestige();
	jewelry_cards_number++;
	// In the main, test if eligible for royal card and call get royal
	// card

	Game::getGame().getCurrentPlayer().applyCapacity(
	    card, Game::getGame().getOpponent());
}

void Player::buyCard_Qt(Draw *t, const int index) {

	// the card to remove is the i-th from draw t

	const JewelryCard &card = t->getCardWithoutDeletion(index);
	if (card.getBonus() == colorBonus::joker &&
	    jokerCardEligibility() == false) {
		throw SplendorException("You are not allowed to buy a card "
		                        "with a joker bonus because "
		                        "you cannot assign the bonus!");
	}

	// here calculating bonus removes the bonus from the total card cost
	// of already owned cards.
	int white_cost =
	    positiveOrNull(card.getCostWhite() - calculateBonus(colorBonus::white));
	int blue_cost =
	    positiveOrNull(card.getCostBlue() - calculateBonus(colorBonus::blue));
	int red_cost =
	    positiveOrNull(card.getCostRed() - calculateBonus(colorBonus::red));
	int green_cost =
	    positiveOrNull(card.getCostGreen() - calculateBonus(colorBonus::green));
	int black_cost =
	    positiveOrNull(card.getCostBlack() - calculateBonus(colorBonus::black));
	int perl_cost = card.getCostPerl();

	// Check if the player wants to use gold tokens (if they have any)
	// And reduce the cost accordingly
	int nb_gold = 0;
	char choices;
	if (TokenAmount(Color::gold) > 0) {

		MainWindow::getMainWindow().triggerYesNo(
		    &choices, "Do you want to use gold token(s)?");

		int nb;
		while ((TokenAmount(Color::gold) > nb_gold) && (choices == 'Y')) {
			// choose the color
			//  retrieve the 2 values
			Color color;

			MainWindow::getMainWindow().triggercolorChoice(&color, &nb);

			try {
				if (nb > TokenAmount(Color::gold) - nb_gold)
					throw SplendorException("You don't have enough gold tokens "
					                        "to spend that many!");
				if ((color == Color::white) && (nb > white_cost))
					throw SplendorException(
					    "The white cost is less than the number of gold tokens "
					    "you "
					    "want to use as joker!");
				if ((color == Color::blue) && (nb > blue_cost))
					throw SplendorException(
					    "The blue cost is less than the number of gold tokens "
					    "you "
					    "want to use as joker!");
				if ((color == Color::red) && (nb > red_cost))
					throw SplendorException(
					    "The red cost is less than the number of gold tokens "
					    "you "
					    "want to use as joker!");
				if ((color == Color::green) && (nb > green_cost))
					throw SplendorException(
					    "The green cost is less than the number of gold tokens "
					    "you "
					    " "
					    "want to use as joker!");
				if ((color == Color::black) && (nb > black_cost))
					throw SplendorException(
					    "The black cost is less than the number of gold tokens "
					    "you "
					    " "
					    "want to use as joker!");
				if ((color == Color::perl) && (nb > white_cost))
					throw SplendorException(
					    "The perl cost is less than the number of gold tokens "
					    "you "
					    "want to use as joker!");
				if ((color != Color::white) && (color != Color::blue) &&
				    (color != Color::red) && (color != Color::green) &&
				    (color != Color::black) && (color != Color::perl))
					throw SplendorException("Unknown token color!");

				// If all checks passed, we can use the gold tokens
				nb_gold += nb;
				if (color == Color::white)
					white_cost = positiveOrNull(white_cost - nb);
				if (color == Color::blue)
					blue_cost = positiveOrNull(blue_cost - nb);
				if (color == Color::red)
					red_cost = positiveOrNull(red_cost - nb);
				if (color == Color::green)
					green_cost = positiveOrNull(green_cost - nb);
				if (color == Color::black)
					black_cost = positiveOrNull(black_cost - nb);
				if (color == Color::perl)
					perl_cost = positiveOrNull(perl_cost - nb);
			} catch (SplendorException e) {
				MainWindow::getMainWindow().triggerInfo(e.getInfo());
				cout << e.getInfo() << endl;
			}
			if (TokenAmount(Color::gold) > nb_gold)
				MainWindow::getMainWindow().triggerYesNo(
				    &choices, "Use another gold token?");
		}
	}
	// Check if enough tokens
	int eligible_purchase = 0;

	// check if we have enough tokens to buy
	if (TokenAmount(Color::white) >= white_cost &&
	    TokenAmount(Color::blue) >= blue_cost &&
	    TokenAmount(Color::red) >= red_cost &&
	    TokenAmount(Color::green) >= green_cost &&
	    TokenAmount(Color::black) >= black_cost &&
	    TokenAmount(Color::perl) >= perl_cost) {
		eligible_purchase = 1;
	}

	if (eligible_purchase == 0)
		throw SplendorException("Not enough tokens to buy the card!");

	// Remove used tokens and put them back in the bag
	withdrawTokens(Color::white, white_cost);
	withdrawTokens(Color::blue, blue_cost);
	withdrawTokens(Color::red, red_cost);
	withdrawTokens(Color::green, green_cost);
	withdrawTokens(Color::black, black_cost);
	withdrawTokens(Color::perl, perl_cost);
	withdrawTokens(Color::gold, nb_gold);

	// Put the card in the player's hand and remove it from the draw
	bought_jewelry_cards.push_back(&(t->getCard(index)));
	try {
		t->fill();
	} catch (SplendorException &e) {
	}

	// Add the crown count
	crown_number += card.getNbCrown();
	points_number += card.getPrestige();
	jewelry_cards_number++;
	// In the main, test if eligible for royal card and call get royal
	// card

	Game::getGame().getCurrentPlayer().applyCapacity_Qt(
	    card, Game::getGame().getOpponent());
	MainWindow::getMainWindow().updateDraws();
}

void Player::buyReservedCard(const int index) {
	if (reserved_jewelry_cards.size() == 0 || index > 3) {
		throw SplendorException("No reserved cards");
	}

	// we must verify that the purchase can be made

	const JewelryCard *card = reserved_jewelry_cards[index];

	if (card->getBonus() == colorBonus::joker and
	    jokerCardEligibility() == false) {
		throw SplendorException("You are not allowed to buy a card "
		                        "with a joker bonus because "
		                        "you cannot assign the bonus!");
	}

	int white_cost = positiveOrNull(card->getCostWhite() -
	                                calculateBonus(colorBonus::white));
	int blue_cost =
	    positiveOrNull(card->getCostBlue() - calculateBonus(colorBonus::blue));
	int red_cost =
	    positiveOrNull(card->getCostRed() - calculateBonus(colorBonus::red));
	int green_cost = positiveOrNull(card->getCostGreen() -
	                                calculateBonus(colorBonus::green));
	int black_cost = positiveOrNull(card->getCostBlack() -
	                                calculateBonus(colorBonus::black));
	int perl_cost = card->getCostPerl();

	int nb_gold = 0;
	if (TokenAmount(Color::gold) > 0) {
		std::cout << "Do you want to use gold token(s)? [Y/N]"
		          << std::endl;
		std::string choices;
		std::cin >> choices;
		int nb;
		if (choices == "Y") {
			while (TokenAmount(Color::gold) > nb_gold) {
				// choose the color
				std::cout << "Color to replace? Stop to stop using "
				             "gold tokens:"
				          << std::endl;
				std::cin >> choices;
				if (choices == "stop" || "Stop") {
					break;
				}
				do {
					// choose the number of gold tokens used for the color in
					// question
					std::cout << "How many?" << std::endl;
					std::cin >> nb;
				} while (nb > TokenAmount(Color::gold));
				try {
					if (nb > TokenAmount(Color::gold) - nb_gold)
						throw SplendorException(
						    "You don't have enough gold tokens to "
						    "spend that many!");
					if ((choices == "white" || "White") && (nb > white_cost))
						throw SplendorException(
						    "The white cost is less than the number of gold "
						    "tokens you "
						    "want to use as joker!");
					if ((choices == "blue" || "Blue") && (nb > blue_cost))
						throw SplendorException(
						    "The blue cost is less than the number of gold tokens "
						    "you "
						    "want to use as joker!");
					if ((choices == "red" || "Red") && (nb > red_cost))
						throw SplendorException(
						    "The red cost is less than the number of gold "
						    "tokens you "
						    "want to use as joker!");
					if ((choices == "green" || "Green") && (nb > green_cost))
						throw SplendorException(
						    "The green cost is less than the number of gold "
						    "tokens "
						    "you "
						    "want to use as joker!");
					if ((choices == "black" || "Black") && (nb > black_cost))
						throw SplendorException(
						    "The black cost is less than the number of gold "
						    "tokens "
						    "you "
						    "want to use as joker!");
					if ((choices == "perl" || "Pearl") && (nb > white_cost))
						throw SplendorException(
						    "The perl cost is less than the number of gold "
						    "tokens you "
						    "want to use as joker!");
					if (choices != "white" || "White" || "blue" || "Blue" ||
					    "red" || "Red" || "green" || "Green" || "black" ||
					    "Black" || "perl" || "Pearl")
						throw SplendorException("Unknown token color!");

					// If all checks passed, we can use the gold
					// tokens
					nb_gold += nb;
					if (choices == "white" || "White")
						white_cost = positiveOrNull(white_cost - nb);
					if (choices == "blue" || "Blue")
						blue_cost = positiveOrNull(blue_cost - nb);
					if (choices == "red" || "Red")
						red_cost = positiveOrNull(red_cost - nb);
					if (choices == "green" || "Green")
						green_cost = positiveOrNull(green_cost - nb);
					if (choices == "black" || "Black")
						black_cost = positiveOrNull(black_cost - nb);
					if (choices == "perl" || "Pearl")
						perl_cost = positiveOrNull(perl_cost - nb);
				} catch (SplendorException e) {
					cout << e.getInfo() << endl;
				}
			}
		}
	}

	// Check if enough tokens
	int eligible_purchase = 0;

	// check if we have enough tokens to buy
	if (TokenAmount(Color::white) >= white_cost &&
	    TokenAmount(Color::blue) >= blue_cost &&
	    TokenAmount(Color::red) >= red_cost &&
	    TokenAmount(Color::green) >= green_cost &&
	    TokenAmount(Color::black) >= black_cost &&
	    TokenAmount(Color::perl) >= perl_cost) {
		eligible_purchase = 1;
	}

	if (eligible_purchase == 0)
		throw SplendorException("Not enough tokens to buy the card!");

	// Remove used tokens and put them back in the bag
	withdrawTokens(Color::white, white_cost);
	withdrawTokens(Color::blue, blue_cost);
	withdrawTokens(Color::red, red_cost);
	withdrawTokens(Color::green, green_cost);
	withdrawTokens(Color::black, black_cost);
	withdrawTokens(Color::perl, perl_cost);
	withdrawTokens(Color::gold, nb_gold);

	reserved_jewelry_cards.erase(reserved_jewelry_cards.begin() + index);
	reserved_jewelry_cards_number--;
	bought_jewelry_cards.push_back(card);
	jewelry_cards_number++;
	crown_number += card->getNbCrown();
	points_number += card->getPrestige();

	Game::getGame().getCurrentPlayer().applyCapacity(
	    *card, Game::getGame().getOpponent());
}

void Player::buyReservedCard_Qt(const int index) {
	if (reserved_jewelry_cards.size() == 0 || index > 3) {
		throw SplendorException("No reserved cards");
	}

	// we must verify that the purchase can be made

	const JewelryCard *card = reserved_jewelry_cards[index];

	if (card->getBonus() == colorBonus::joker and
	    jokerCardEligibility() == false) {
		throw SplendorException("You are not allowed to buy a card "
		                        "with a joker bonus because "
		                        "you cannot assign the bonus!");
	}

	int white_cost = positiveOrNull(card->getCostWhite() -
	                                calculateBonus(colorBonus::white));
	int blue_cost =
	    positiveOrNull(card->getCostBlue() - calculateBonus(colorBonus::blue));
	int red_cost =
	    positiveOrNull(card->getCostRed() - calculateBonus(colorBonus::red));
	int green_cost = positiveOrNull(card->getCostGreen() -
	                                calculateBonus(colorBonus::green));
	int black_cost = positiveOrNull(card->getCostBlack() -
	                                calculateBonus(colorBonus::black));
	int perl_cost = card->getCostPerl();

	int nb_gold = 0;
	if (TokenAmount(Color::gold) > 0) {

		char choices;
		MainWindow::getMainWindow().triggerYesNo(
		    &choices, "Do you want to use gold token(s)?");

		int nb;

		while ((TokenAmount(Color::gold) > nb_gold) && (choices == 'Y')) {
			// choose the color
			//  retrieve the 2 values
			Color color;

			MainWindow::getMainWindow().triggercolorChoice(&color, &nb);

			try {
				if (nb > TokenAmount(Color::gold) - nb_gold)
					throw SplendorException("You don't have enough gold tokens "
					                        "to spend that many!");
				if ((color == Color::white) && (nb > white_cost))
					throw SplendorException(
					    "The white cost is less than the number of gold tokens "
					    "you "
					    "want to use as joker!");
				if ((color == Color::blue) && (nb > blue_cost))
					throw SplendorException(
					    "The blue cost is less than the number of gold tokens "
					    "you "
					    "want to use as joker!");
				if ((color == Color::red) && (nb > red_cost))
					throw SplendorException(
					    "The red cost is less than the number of gold tokens "
					    "you "
					    "want to use as joker!");
				if ((color == Color::green) && (nb > green_cost))
					throw SplendorException(
					    "The green cost is less than the number of gold tokens "
					    "you "
					    " "
					    "want to use as joker!");
				if ((color == Color::black) && (nb > black_cost))
					throw SplendorException(
					    "The black cost is less than the number of gold tokens "
					    "you "
					    " "
					    "want to use as joker!");
				if ((color == Color::perl) && (nb > white_cost))
					throw SplendorException(
					    "The perl cost is less than the number of gold tokens "
					    "you "
					    "want to use as joker!");
				if ((color != Color::white) && (color != Color::blue) &&
				    (color != Color::red) && (color != Color::green) &&
				    (color != Color::black) && (color != Color::perl))
					throw SplendorException("Unknown token color!");

				// If all checks passed, we can use the gold tokens
				nb_gold += nb;
				if (color == Color::white)
					white_cost = positiveOrNull(white_cost - nb);
				if (color == Color::blue)
					blue_cost = positiveOrNull(blue_cost - nb);
				if (color == Color::red)
					red_cost = positiveOrNull(red_cost - nb);
				if (color == Color::green)
					green_cost = positiveOrNull(green_cost - nb);
				if (color == Color::black)
					black_cost = positiveOrNull(black_cost - nb);
				if (color == Color::perl)
					perl_cost = positiveOrNull(perl_cost - nb);
			} catch (SplendorException e) {
				MainWindow::getMainWindow().triggerInfo(e.getInfo());
				cout << e.getInfo() << endl;
			}
			if (TokenAmount(Color::gold) > nb_gold)
				MainWindow::getMainWindow().triggerYesNo(
				    &choices, "Use another gold token?");
		}
	}

	// Check if enough tokens
	int eligible_purchase = 0;

	// check if we have enough tokens to buy
	if (TokenAmount(Color::white) >= white_cost &&
	    TokenAmount(Color::blue) >= blue_cost &&
	    TokenAmount(Color::red) >= red_cost &&
	    TokenAmount(Color::green) >= green_cost &&
	    TokenAmount(Color::black) >= black_cost &&
	    TokenAmount(Color::perl) >= perl_cost) {
		eligible_purchase = 1;
	}

	if (eligible_purchase == 0)
		throw SplendorException("Not enough tokens to buy the card!");

	// Remove used tokens and put them back in the bag
	withdrawTokens(Color::white, white_cost);
	withdrawTokens(Color::blue, blue_cost);
	withdrawTokens(Color::red, red_cost);
	withdrawTokens(Color::green, green_cost);
	withdrawTokens(Color::black, black_cost);
	withdrawTokens(Color::perl, perl_cost);
	withdrawTokens(Color::gold, nb_gold);

	reserved_jewelry_cards.erase(reserved_jewelry_cards.begin() + index);
	reserved_jewelry_cards_number--;
	bought_jewelry_cards.push_back(card);
	jewelry_cards_number++;
	crown_number += card->getNbCrown();
	points_number += card->getPrestige();

	Game::getGame().getCurrentPlayer().applyCapacity_Qt(
	    *card, Game::getGame().getOpponent());
}

void Player::royalCardSelection() {
	cout << "Your crown count allows you to draw a royal "
	        "card!"
	     << endl;
	Game::getGame().printRoyalCards();
	int tmp;
	bool choice_ok = 0;
	while (!choice_ok) {
		cout << "Please enter the index of the royal card you want "
		        "to take:"
		     << endl;
		cout << "choices: ";
		cin >> tmp;
		if (tmp >= 0 and tmp < Game::getGame().getRoyalCards().size()) {
			string validate;
			cout << "Confirm? [Y/N]:" << endl;
			cout << "choices : ";
			cin >> validate;
			if (validate == "Y") {
				choice_ok = 1; // we can exit the loop
			}
		} else {
			cout << "Invalid card index!" << endl;
		}
	}
	obtainRoyalCard(tmp);
}

// Qt overload

void Player::royalCardSelection_Qt() {

	MainWindow::getMainWindow().activateForRoyalCard();
	MainWindow::getMainWindow().getCardWaitLoop()->exec();

	Qt_card *last_card_clicked =
	    MainWindow::getMainWindow().getLastCardClicked();

	int tmp =
	    last_card_clicked
	        ->getIndex(); // Retrieve the index using a signal here

	obtainRoyalCard_qt(tmp);
}

void Player::tokenVerification() {
	if (token_number > 10) {
		int nb = token_number - 10;
		cout << "You have too many tokens, you must return " << nb
		     << " to the bag." << endl;
		cout << "Here are your tokens:" << endl;
		for (int i = 0; i < token_number; i++) {
			cout << "Index " << i << " : " << *tokens[i] << endl;
		}
		vector<int> tab(0);
		int i = 0;
		int curNb = nb;
		while (curNb > 0) {
			int tmp;
			cout << "Token index " << i++ << ":" << endl;
			cout << "Choice: ";
			cin >> tmp;
			bool inTab = 0;
			for (int j = 0; j < tab.size();
			     j++) { // check if we haven't already tried to remove it
				if (tab[j] == tmp) {
					inTab = 1;
				}
			}
			if (tmp >= 0 and tmp < token_number and inTab == false) {
				tab.push_back(tmp);
				curNb--;
			}
		}
		// sort the array
		std::make_heap(tab.begin(), tab.end());
		std::sort_heap(tab.begin(), tab.end());
		for (int k = tab.size() - 1; k >= 0;
		     k--) { // remove from end to start to avoid shifting
			        // indices
			Bag::get().insertToken(tokens[tab[k]]);
			tokens.erase(tokens.begin() + tab[k]);
			token_number--;
		}
	}
}

void Player::tokenVerification_Qt() {
	if (token_number > 10) {
		MainWindow::getMainWindow().setDiscarding(true);
		MainWindow::getMainWindow().deactivateButtons();
		int nb = token_number - 10;
		MainWindow::getMainWindow().triggerInfo("You must remove " +
		                                        std::to_string(nb) + " tokens");

		vector<int> tab(0);
		int i = 0;
		int curNb = nb;
		while (curNb > 0) {

			MainWindow::getMainWindow().getTokenWaitLoop()->exec();
			int tmp = MainWindow::getMainWindow().getTokenIndexOnClick();

			bool inTab = 0;
			for (int j = 0; j < tab.size();
			     j++) { // check if we haven't already tried to remove it
				if (tab[j] == tmp) {
					inTab = 1;
				}
			}
			if (tmp >= 0 and tmp < token_number and inTab == false) {
				tab.push_back(tmp);
				curNb--;
			}
		}
		MainWindow::getMainWindow().acceptCurrentDialog();
		// sort the array
		std::make_heap(tab.begin(), tab.end());
		std::sort_heap(tab.begin(), tab.end());
		for (int k = tab.size() - 1; k >= 0;
		     k--) { // remove from end to start to avoid shifting
			        // indices
			Bag::get().insertToken(tokens[tab[k]]);
			tokens.erase(tokens.begin() + tab[k]);
			token_number--;
		}
	}

	MainWindow::getMainWindow().setDiscarding(false);
}
/******************** Player ********************/

/******************** RandomPlayer - aka IA ********************/

RandomPlayer::RandomPlayer(const string &name) : StrategyPlayer(name) {}

RandomPlayer::RandomPlayer(const json data) : StrategyPlayer(data) {}

RandomPlayer::~RandomPlayer() {
	// Destroy royal cards
	for (auto royal_cards : royal_cards) {
		delete royal_cards;
	}
	royal_cards.clear();

	// Destroy privileges
	for (auto privilege : privileges) {
		delete privilege;
	}
	privileges.clear();
}

// Polymorphic methods adapted for a RandomPlayer
void RandomPlayer::choice() {
	cout << "choices RandomPlayer:" << endl;
	bool end_choice = 0;
	int nb_choice = 0;
	while (!end_choice) {
		try {
			int i = 0;
			nb_choice =
			    getOptionalChoices(); // check the convention on the
			                          // return in the method definition
			i += nb_choice + 2;
			int tmp = rand() % i + 1;

			if (tmp < 1 or tmp > i + 1) {
				throw SplendorException(
				    "There are only " + to_string(i) +
				    " choices! You cannot choose anything else!\n");
			}
			switch (nb_choice) { // the display and therefore the choice depends on the
				                 // return value of the optional choices
			case (0): {          // no optional choice possible
				switch (tmp) {
				case 1: {
					cout << "The RandomPlayer selects tokens!" << endl;
					tokenSelection();
					end_choice = 1;
					break;
				}
				case 2: {
					cout << "The RandomPlayer buys cards!" << endl;
					cardPurchase();
					end_choice = 1;

					break;
				}
				default: // continue until the user chooses
				         // a valid entry!
					break;
				}
				break;
			}
			case (1): { // only possible to use a privilege
				switch (tmp) {
				case 1: {
					cout << "The RandomPlayer uses a privilege!" << endl;
					usePrivilege();
					break;
				}
				case 2: {
					cout << "The RandomPlayer selects tokens!" << endl;
					tokenSelection();
					end_choice = 1;
					break;
				}
				case 3: {
					cout << "The RandomPlayer buys cards!" << endl;
					cardPurchase();
					end_choice = 1;
					break;
				}
				default:
					break;
				}
				break;
			}
			case (2): { // only possible to fill the board
				switch (tmp) {
				case 1: { // fill board
					cout << "Le RandomPlayer remplie le board!" << endl;
					fillingBoard();
					break;
				}
				case 2: {
					cout << "The RandomPlayer selects tokens!" << endl;
					tokenSelection();
					end_choice = 1;
					break;
				}
				case 3: {
					cout << "The RandomPlayer buys cards!" << endl;
					cardPurchase();
					end_choice = 1;
					break;
				}
				default:
					break;
				}
				break;
			}
			case (3): { // two optional choices possible
				switch (tmp) {
				case 1: {
					cout << "The RandomPlayer uses a privilege!" << endl;
					usePrivilege();
					break;
				}
				case 2: { // fill board
					cout << "Le RandomPlayer remplie le board!" << endl;
					fillingBoard();
					break;
				}
				case 3: {
					cout << "The RandomPlayer selects tokens!" << endl;
					tokenSelection();
					end_choice = 1;
					break;
				}
				case 4: {
					cout << "The RandomPlayer buys cards!" << endl;
					cardPurchase();
					end_choice = 1;
					break;
				}
				default:
					break;
				}
				break;
			}
			default:
				break;
			}

		} catch (SplendorException &e) {
			cout << e.getInfo() << "\n";
		}
	}
}

// Qt overload

void RandomPlayer::choice_Qt() {
	cout << "choices RandomPlayer:" << endl;
	bool end_choice = 0;
	int nb_choice = 0;
	while (!end_choice) {
		QCoreApplication::processEvents();
		try {
			int i = 0;
			nb_choice =
			    getOptionalChoices(); // check the convention on the
			                          // return in the method definition
			i += nb_choice + 2;
			int tmp = rand() % i + 1;

			if (tmp < 1 or tmp > i + 1) {
				throw SplendorException(
				    "There are only " + to_string(i) +
				    " choices! You cannot choose anything else!\n");
			}
			QCoreApplication::processEvents();
			switch (nb_choice) { // the display and therefore the choice depends on the
				                 // return value of the optional choices
			case (0): {          // no optional choice possible
				switch (tmp) {
				case 1: {
					cout << "The RandomPlayer selects tokens!" << endl;
					tokenSelection();
					end_choice = 1;
					break;
				}
				case 2: {
					cout << "The RandomPlayer buys cards!" << endl;
					cardPurchase();
					end_choice = 1;
					break;
				}
				default: // continue until the user chooses
				         // a valid entry!
					break;
				}
				break;
			}
			case (1): { // only possible to use a privilege
				switch (tmp) {
				case 1: {
					cout << "The RandomPlayer uses a privilege!" << endl;
					usePrivilege();
					break;
				}
				case 2: {
					cout << "The RandomPlayer selects tokens!" << endl;
					tokenSelection();
					end_choice = 1;
					break;
				}
				case 3: {
					cout << "The RandomPlayer buys cards!" << endl;
					cardPurchase();
					end_choice = 1;
					break;
				}
				default:
					break;
				}
				break;
			}
			case (2): { // only possible to fill the board
				switch (tmp) {
				case 1: { // fill board
					cout << "Le RandomPlayer remplie le board!" << endl;
					fillingBoard();
					break;
				}
				case 2: {
					cout << "The RandomPlayer selects tokens!" << endl;
					tokenSelection();
					end_choice = 1;
					break;
				}
				case 3: {
					cout << "The RandomPlayer buys cards!" << endl;
					cardPurchase();
					end_choice = 1;
					break;
				}
				default:
					break;
				}
				break;
			}
			case (3): { // two optional choices possible
				switch (tmp) {
				case 1: {
					cout << "The RandomPlayer uses a privilege!" << endl;
					usePrivilege();
					break;
				}
				case 2: { // fill board
					cout << "Le RandomPlayer remplie le board!" << endl;
					fillingBoard();
					break;
				}
				case 3: {
					cout << "The RandomPlayer selects tokens!" << endl;
					tokenSelection();
					end_choice = 1;
					break;
				}
				case 4: {
					cout << "The RandomPlayer buys cards!" << endl;
					cardPurchase();
					end_choice = 1;
					break;
				}
				default:
					break;
				}
				break;
			}
			default: {
				break;
			}
			}
			MainWindow::getMainWindow().updateBoard();
			MainWindow::getMainWindow().updateDraws();
			MainWindow::getMainWindow().updateScores();
			MainWindow::getMainWindow().updatePrivileges();
			MainWindow::getMainWindow().update();

		} catch (SplendorException &e) {
			cout << e.getInfo() << "\n";
		}
	}
}

void RandomPlayer::usePrivilege() {
	if (privilege_number <= 0)
		throw SplendorException("You do not have a privilege!");
	unsigned int index = rand() % Token::getMaxTokenNumber();
	Game::getGame().getPlayerRound().drawToken(index);
	withdrawPrivilege();
}

void RandomPlayer::tokenSelection() {
	bool nb_ok = 0;
	bool choice_ok = 0;

	if (Board::getBoard().onlyGold() and
	    Game::getGame().getPlayerRound().getReservedCardNumber() >= 3)
		throw SplendorException(
		    "Cannot draw: only gold tokens remain and you cannot "
		    "reserve any more cards!");
	if (Board::getBoard().getCurrentNb() == 0)
		throw SplendorException("Board vide!");
	while (!choice_ok) {
		try {
			std::vector<int> tmp_tab(0);
			optional<Position> pos = nullopt;
			int gold_number = 0;
			int perl_number = 0;
			string validation;
			while (tmp_tab.size() < 3) {
				unsigned int index;
				index = rand() % Token::getMaxTokenNumber();

				if (tmp_tab.size() > 0) { // active only after the 1st choice
					if (rand() % 2 == 0) {
						break;
					}
				}
				if (Board::getBoard().getBoardCaseByIndex(index) ==
				    nullptr) { // the number of cells on the board corresponds
					           // to the number of tokens in the game
					gold_number = 0;
					perl_number = 0;
					tmp_tab.clear();

					throw SplendorException(
					    "There is no token at this index!\n");
				}
				if (index >
				    Token::getMaxTokenNumber()) { // the number of cells on the
					                              // board corresponds to the
					                              // number of tokens in the game
					gold_number = 0;
					perl_number = 0;
					tmp_tab.clear();
					throw SplendorException(
					    "There are only " +
					    std::to_string(Token::getMaxTokenNumber()) +
					    " places on the board\n");
				}
				if (Board::getBoard().getBoardCaseByIndex(index)->getColor() ==
				    Color::gold) {
					gold_number++;
				}
				if (Board::getBoard().getBoardCaseByIndex(index)->getColor() ==
				    Color::perl) {
					perl_number++;
				}
				if ((gold_number == 1) &&
				    (Game::getGame().getPlayerRound().getReservedCardNumber() >=
				     3)) {
					gold_number = 0;
					perl_number = 0;
					tmp_tab.clear();
					throw SplendorException(
					    "You are not allowed to reserve an additional "
					    "card!");
				}
				if (gold_number == 1 and tmp_tab.size() > 0) {
					gold_number = 0;
					perl_number = 0;
					tmp_tab.clear();
					throw SplendorException("Warning, you can only take a "
					                        "gold token by itself!");
				}
				tmp_tab.push_back(index);
			}

			// sort the vector by selection
			int min = 0;
			for (int j = 0; j < tmp_tab.size() - 1; j++) {
				min = j;
				for (int k = j + 1; k < tmp_tab.size(); k++) {
					if (tmp_tab[k] < tmp_tab[min]) {
						min = k;
					}
				}
				if (min != j) {
					int tmp2 = tmp_tab[min];
					tmp_tab[min] = tmp_tab[j];
					tmp_tab[j] = tmp2;
				}
			}
			// sorted vector
			// cout<<"sorted vector\n";

			if (tmp_tab.size() ==
			    2) { // alignment check for 2 tokens
				// cout<<"check for 2 tokens\n";
				const Token *tok1 =
				    Board::getBoard().getBoardCaseByIndex(tmp_tab[0]);
				optional<Position> pos1 =
				    Board::getBoard().tokensAreASide(tmp_tab[1], tok1);
				if (pos1 == nullopt) {
					throw SplendorException("Tokens not aligned\n");
				}
			}
			if (tmp_tab.size() ==
			    3) { // alignment check for 3 tokens
				// cout<<"check for 3 tokens\n";
				const Token *tok1 =
				    Board::getBoard().getBoardCaseByIndex(tmp_tab[1]);
				optional<Position> pos1 =
				    Board::getBoard().tokensAreASide(tmp_tab[0], tok1);

				const Token *tok2 =
				    Board::getBoard().getBoardCaseByIndex(tmp_tab[2]);
				optional<Position> pos2 =
				    Board::getBoard().tokensAreASide(tmp_tab[1], tok2);

				if ((pos1 != pos2) || (pos1 == nullopt) || (pos2 == nullopt)) {
					throw SplendorException("Tokens not aligned\n");
				}
			}

			// we have verified the token alignment
			if (perl_number ==
			    2) { // opponent gets a privilege if
				     // we draw 2 perl tokens at once
				Game::getGame().getOpponent().obtainPrivilege();
			}

			if (tmp_tab.size() ==
			    3) { // opponent gets a privilege if
				     // all 3 tokens are the same color
				if ((Board::getBoard()
				         .getBoardCaseByIndex(tmp_tab[0])
				         ->getColor() == Board::getBoard()
				                             .getBoardCaseByIndex(tmp_tab[1])
				                             ->getColor()) &&
				    (Board::getBoard()
				         .getBoardCaseByIndex(tmp_tab[1])
				         ->getColor() == Board::getBoard()
				                             .getBoardCaseByIndex(tmp_tab[2])
				                             ->getColor())) {
					Game::getGame().getOpponent().obtainPrivilege();
				}
			}
			for (int i = 0; i < tmp_tab.size();
			     i++) { // token acquisition by the player
				Game::getGame().getCurrentPlayer().drawToken(tmp_tab[i]);
			}
			if (gold_number == 1) {
				cardReservation();
			}
			choice_ok = 1;
		} catch (SplendorException &e) {
			cout << e.getInfo() << "\n";
		}
	}
}

void RandomPlayer::cardReservation() {
	if (rand() % 2 == 0) { // reserve from a draw
		int draw = rand() % Deck::getDecksNumber() +
		           1; // we assume the number of decks equals the number
		              // of draws (between 1 and 3)
		switch (draw) {
		case 1: {
			unsigned int index =
			    rand() % Game::getGame().getFirstDraw()->getCardsNumber();
			reserveCard(Game::getGame().getFirstDraw(), index);
			break;
		}
		case 2: {
			unsigned int index =
			    rand() % Game::getGame().getSecondDraw()->getCardsNumber();
			reserveCard(Game::getGame().getSecondDraw(), index);
			break;
		}
		case 3: {
			unsigned int index =
			    rand() % Game::getGame().getThirdDraw()->getCardsNumber();
			reserveCard(Game::getGame().getThirdDraw(), index);
			break;
		}
		}
	} else { // reserve from a deck
		int deck = rand() % Deck::getDecksNumber() + 1; // between 1 and 3
		reserveCard(Game::getGame().getDeck(deck));
	}
	for (auto c : Game::getGame().getCurrentPlayer().getReservedCards()) {
		cout << *c << endl;
	}
}

void RandomPlayer::cardPurchase() {
	unsigned int choice = -1;
	if (Game::getGame().getCurrentPlayer().getReservedCardNumber() >
	    0) { // if the player has reserved cards
		choice = rand() % 2;
		if (choice == 1) { // buy a reserved card
			cout << "\nBuying a reserved card!" << endl;
			int nb_reserved_cards =
			    Game::getGame().getCurrentPlayer().getReservedCards().size();
			unsigned int index = rand() % nb_reserved_cards;
			Game::getGame().getCurrentPlayer().buyReservedCard(index);
		} else {
			cout << "\nBuying a card from a draw!" << endl;
			int draw = (rand() % Deck::getDecksNumber()) + 1; // between 1 and 3
			switch (draw) {
			case 1: {
				cout << "Buying a card from draw 1." << endl;
				unsigned int numCardsT1 =
				    Game::getGame().getFirstDraw()->getCardsNumber();
				if (numCardsT1 == 0)
					throw SplendorException("No more cards in Draw 1");
				unsigned int index = rand() % numCardsT1;
				cout << *Game::getGame().getFirstDraw()->getDrawCards()[index]
				     << endl;
				Game::getGame().getCurrentPlayer().buyCard(
				    Game::getGame().getFirstDraw(), index);
				break;
			}
			case 2: {
				cout << "Buying a card from draw 2." << endl;
				unsigned int numCardsT2 =
				    Game::getGame().getSecondDraw()->getCardsNumber();
				if (numCardsT2 == 0)
					throw SplendorException("No more cards in Draw 2");
				unsigned int index = rand() % numCardsT2;
				cout << *Game::getGame().getSecondDraw()->getDrawCards()[index]
				     << endl;
				Game::getGame().getCurrentPlayer().buyCard(
				    Game::getGame().getSecondDraw(), index);
				break;
			}
			case 3: {
				cout << "Buying a card from draw 3." << endl;
				unsigned int numCardsT3 =
				    Game::getGame().getThirdDraw()->getCardsNumber();
				if (numCardsT3 == 0)
					throw SplendorException("No more cards in Draw 3");
				unsigned int index = rand() % numCardsT3;
				cout << *Game::getGame().getThirdDraw()->getDrawCards()[index]
				     << endl;
				Game::getGame().getCurrentPlayer().buyCard(
				    Game::getGame().getThirdDraw(), index);
				break;
			}
			}
		}
	} else { // if we can only buy
		cout << "\nBuying a card from a draw!" << endl;
		int draw = (rand() % Deck::getDecksNumber()) + 1; // between 1 and 3
		switch (draw) {
		case 1: {
			cout << "Buying a card from draw 1." << endl;
			unsigned int numCardsT1 =
			    Game::getGame().getFirstDraw()->getCardsNumber();
			if (numCardsT1 == 0)
				throw SplendorException("No more cards in Draw 1");
			unsigned int index = rand() % numCardsT1;
			cout << *Game::getGame().getFirstDraw()->getDrawCards()[index] << endl;
			Game::getGame().getCurrentPlayer().buyCard(
			    Game::getGame().getFirstDraw(), index);
			break;
		}
		case 2: {
			cout << "Buying a card from draw 2." << endl;
			unsigned int numCardsT2 =
			    Game::getGame().getSecondDraw()->getCardsNumber();
			if (numCardsT2 == 0)
				throw SplendorException("No more cards in Draw 2");
			unsigned int index = rand() % numCardsT2;
			cout << *Game::getGame().getSecondDraw()->getDrawCards()[index]
			     << endl;
			Game::getGame().getCurrentPlayer().buyCard(
			    Game::getGame().getSecondDraw(), index);
			break;
		}
		case 3: {
			cout << "Buying a card from draw 3." << endl;
			unsigned int numCardsT3 =
			    Game::getGame().getThirdDraw()->getCardsNumber();
			if (numCardsT3 == 0)
				throw SplendorException("Plus de card de le Draw 3");
			unsigned int index = rand() % numCardsT3;
			cout << *Game::getGame().getThirdDraw()->getDrawCards()[index] << endl;
			Game::getGame().getCurrentPlayer().buyCard(
			    Game::getGame().getThirdDraw(), index);
			break;
		}
		}
	}
}

void RandomPlayer::buyCard(Draw *t, const int index) {
	const JewelryCard &card = t->getCardWithoutDeletion(index);

	if (card.getBonus() == colorBonus::joker and
	    jokerCardEligibility() == false) {
		throw SplendorException("You are not allowed to buy a card "
		                        "with a joker bonus because "
		                        "you cannot assign the bonus!");
	}

	// here calculating bonus removes the bonus from the total card cost
	// of already owned cards.
	int white_cost =
	    positiveOrNull(card.getCostWhite() - calculateBonus(colorBonus::white));
	int blue_cost =
	    positiveOrNull(card.getCostBlue() - calculateBonus(colorBonus::blue));
	int red_cost =
	    positiveOrNull(card.getCostRed() - calculateBonus(colorBonus::red));
	int green_cost =
	    positiveOrNull(card.getCostGreen() - calculateBonus(colorBonus::green));
	int black_cost =
	    positiveOrNull(card.getCostBlack() - calculateBonus(colorBonus::black));
	int perl_cost = card.getCostPerl();

	// Check if the player wants to use gold tokens (if they have any)
	// And reduce the cost accordingly
	int nb_gold = 0;
	if (TokenAmount(Color::gold) > 0) {
		while (TokenAmount(Color::gold) > nb_gold and
		       rand() % 2 ==
		           1) { // random stop between choices
			            // (possibility of not using all tokens)
			// choose the color
			if (rand() % 2 == 1) { // we stop here
				break;
			}

			// choose the number of gold tokens used for the color in question
			int nb = rand() % TokenAmount(Color::gold) - nb_gold;
			int choices = rand() % 6;
			if (choices == 0 and white_cost >= nb)
				white_cost -= nb;
			nb_gold += nb;
			if (choices == 1 and blue_cost >= nb)
				blue_cost -= nb;
			nb_gold += nb;
			if (choices == 2 and red_cost >= nb)
				red_cost -= nb;
			nb_gold += nb;
			if (choices == 3 and green_cost >= nb)
				green_cost -= nb;
			nb_gold += nb;
			if (choices == 4 and black_cost >= nb)
				black_cost -= nb;
			nb_gold += nb;
			if (choices == 5 and perl_cost >= nb)
				perl_cost -= nb;
			nb_gold += nb;
			// if no good choices relative to cost, the AI retries
		}
	}
	// Check if enough tokens
	int eligible_purchase = 0;

	// check if we have enough tokens to buy
	if (TokenAmount(Color::white) >= white_cost &&
	    TokenAmount(Color::blue) >= blue_cost &&
	    TokenAmount(Color::red) >= red_cost &&
	    TokenAmount(Color::green) >= green_cost &&
	    TokenAmount(Color::black) >= black_cost &&
	    TokenAmount(Color::perl) >= perl_cost) {
		eligible_purchase = 1;
	}

	if (eligible_purchase == 0)
		throw SplendorException("Not enough tokens to buy the card!");

	// Remove used tokens and put them back in the bag
	withdrawTokens(Color::white, white_cost);
	withdrawTokens(Color::blue, blue_cost);
	withdrawTokens(Color::red, red_cost);
	withdrawTokens(Color::green, green_cost);
	withdrawTokens(Color::black, black_cost);
	withdrawTokens(Color::perl, perl_cost);
	withdrawTokens(Color::gold, nb_gold);

	// Put the card in the player's hand and remove it from the draw
	bought_jewelry_cards.push_back(&(t->getCard(index)));
	try {
		t->fill();
	} catch (SplendorException &e) {
	}

	// Add the crown count
	crown_number += card.getNbCrown();
	jewelry_cards_number++;
	points_number += card.getPrestige();
	// In the main, test if eligible for royal card and call get royal
	// card

	Game::getGame().getCurrentPlayer().applyCapacity(
	    card, Game::getGame().getOpponent());
}

void RandomPlayer::buyReservedCard(const int index) {
	if (reserved_jewelry_cards.size() == 0 || index > 3) {
		throw SplendorException("No reserved cards");
	}

	// we must verify that the purchase can be made

	const JewelryCard *card = reserved_jewelry_cards[index];

	if (card->getBonus() == colorBonus::joker and
	    jokerCardEligibility() == false) {
		throw SplendorException("You are not allowed to buy a card "
		                        "with a joker bonus because "
		                        "you cannot assign the bonus!");
	}

	int white_cost = positiveOrNull(card->getCostWhite() -
	                                calculateBonus(colorBonus::white));
	int blue_cost =
	    positiveOrNull(card->getCostBlue() - calculateBonus(colorBonus::blue));
	int red_cost =
	    positiveOrNull(card->getCostRed() - calculateBonus(colorBonus::red));
	int green_cost = positiveOrNull(card->getCostGreen() -
	                                calculateBonus(colorBonus::green));
	int black_cost = positiveOrNull(card->getCostBlack() -
	                                calculateBonus(colorBonus::black));
	int perl_cost = card->getCostPerl();

	int nb_gold = 0;
	if (TokenAmount(Color::gold) > 0) {
		while ((TokenAmount(Color::gold) > nb_gold) and
		       rand() % 2 ==
		           1) { // random stop between choices
			            // (possibility of not using all tokens)
			// choose the color
			if (rand() % 2 == 1) { // we stop here
				break;
			}

			// choose the number of gold tokens used for the color in question
			int nb = rand() % TokenAmount(Color::gold) - nb_gold;

			int choices = rand() % 6;
			if (choices == 0 and white_cost >= nb)
				white_cost -= nb;
			nb_gold += nb;
			if (choices == 1 and blue_cost >= nb)
				blue_cost -= nb;
			nb_gold += nb;
			if (choices == 2 and red_cost >= nb)
				red_cost -= nb;
			nb_gold += nb;
			if (choices == 3 and green_cost >= nb)
				green_cost -= nb;
			nb_gold += nb;
			if (choices == 4 and black_cost >= nb)
				black_cost -= nb;
			nb_gold += nb;
			if (choices == 5 and perl_cost >= nb)
				perl_cost -= nb;
			nb_gold += nb;
			// if no good choices relative to cost, the AI retries
		}
	}

	// Check if enough tokens
	int eligible_purchase = 0;

	// check if we have enough tokens to buy
	if (TokenAmount(Color::white) >= white_cost &&
	    TokenAmount(Color::blue) >= blue_cost &&
	    TokenAmount(Color::red) >= red_cost &&
	    TokenAmount(Color::green) >= green_cost &&
	    TokenAmount(Color::black) >= black_cost &&
	    TokenAmount(Color::perl) >= perl_cost) {
		eligible_purchase = 1;
	}

	if (eligible_purchase == 0)
		throw SplendorException("Not enough tokens to buy the card!");

	// Remove used tokens and put them back in the bag
	withdrawTokens(Color::white, white_cost);
	withdrawTokens(Color::blue, blue_cost);
	withdrawTokens(Color::red, red_cost);
	withdrawTokens(Color::green, green_cost);
	withdrawTokens(Color::black, black_cost);
	withdrawTokens(Color::perl, perl_cost);
	withdrawTokens(Color::gold, nb_gold);

	reserved_jewelry_cards.erase(reserved_jewelry_cards.begin() + index);
	reserved_jewelry_cards_number--;
	bought_jewelry_cards.push_back(card);
	jewelry_cards_number++;
	points_number += card->getPrestige();
	this->crown_number += card->getNbCrown();

	Game::getGame().getCurrentPlayer().applyCapacity(
	    *card, Game::getGame().getOpponent());
}

void RandomPlayer::royalCardSelection() {
	int nb_royal_cards_in_game = Game::getGame().getRoyalCards().size();
	int tmp = rand() % nb_royal_cards_in_game;
	obtainRoyalCard(tmp);
}

void RandomPlayer::applyCapacity(const JewelryCard &card,
                                 StrategyPlayer &opponent) {
	if (card.getCapacity().has_value()) {
		std::optional<Capacity> capa = card.getCapacity();
		if (capa == Capacity::steal_opponent_pawn) {
			if (capa == Capacity::steal_opponent_pawn) {
				cout << "The RandomPlayer can steal a token!\n";
				if (opponent.getToken().empty()) {
					cout << "Too bad, you don't have any gem or perl token!"
					     << endl;
				} else {
					vector<const Token *> opponent_tokens =
					    opponent.getToken();
					int choice;
					do {
						choice = rand() % opponent_tokens.size();
					} while (opponent_tokens[choice]->getColor() ==
					         Color::gold);
					cout << "Stolen token:" << *opponent_tokens[choice] << endl;
					tokens.push_back(opponent_tokens[choice]);
					token_number++;
					opponent_tokens.erase(opponent_tokens.begin() + choice);
					opponent.setNbTokens(opponent.getTokenNumber() - 1);
				}
			}
		} else if (capa == Capacity::take_priviledge) {
			cout << "Capacity usage: the RandomPlayer gains a "
			        "privilege\n";
			Game::getGame().getCurrentPlayer().obtainPrivilege();
		} else if (capa == Capacity::take_on_board) {
			cout << "Capacity usage: the RandomPlayer can draw a "
			        "token of the "
			        "card's bonus color\n";
			const optional<enum colorBonus> &color = card.getBonus();
			if (Board::getBoard().colorsOnBoard(color)) {
				vector<int> valid_indexes =
				    Board::getBoard().getTokenIndexesByColor(color);

				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> distribution(
				    0, valid_indexes.size() - 1);
				int index = distribution(gen);
				int choice = valid_indexes[index];

				try {
					Game::getGame().getCurrentPlayer().drawToken(choice);
				} catch (SplendorException &e) {
					cout << e.getInfo() << "\n";
				}
			} else {
				cout << "Too bad, there are no more tokens of this color on "
				        "the "
				        "board."
				     << endl;
			}
		} else {
			Game::getGame().nextRound(1);
			cout << "The RandomPlayer replays (capacity of the card just "
			        "bought)!"
			     << endl;
		}
	}
	if (card.getBonus() == colorBonus::joker) {
		cout << "Capacity usage: the RandomPlayer transforms the joker "
		        "into a "
		        "color "
		        "bonus by associating it with"
		        " one of your cards with at least one bonus.\n";
		int bonus_white = calculateBonus(colorBonus::white);
		int bonus_blue = calculateBonus(colorBonus::blue);
		int bonus_red = calculateBonus(colorBonus::red);
		int bonus_green = calculateBonus(colorBonus::green);
		int bonus_black = calculateBonus(colorBonus::black);

		try {
			vector<int> possible_choices;
			int option = 0;
			if (bonus_white > 0) {
				possible_choices.push_back(1);
				option++;
			}
			if (bonus_blue > 0) {
				possible_choices.push_back(2);
				option++;
			}
			if (bonus_red > 0) {
				possible_choices.push_back(3);
				option++;
			}
			if (bonus_green > 0) {
				possible_choices.push_back(4);
				option++;
			}
			if (bonus_black > 0) {
				possible_choices.push_back(5);
				option++;
			}
			// Check if no option is available
			if (option == 0) {
				throw SplendorException(
				    "you don't own any card with a bonus.. Capacity"
				    " has no effect\n");
			} else {
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<> distribution(
				    0, possible_choices.size() - 1);
				int index = distribution(gen);
				int choices = possible_choices[index];

				colorBonus b;
				switch (choices) {
				case 1:
					b = colorBonus::white;
					card.changeBonusColor(b);
					break;
				case 2:
					b = colorBonus::blue;
					card.changeBonusColor(b);
					break;
				case 3:
					b = colorBonus::red;
					card.changeBonusColor(b);
					break;
				case 4:
					b = colorBonus::green;
					card.changeBonusColor(b);
					break;
				case 5:
					b = colorBonus::black;
					card.changeBonusColor(b);
					break;
				default:
					cout << "Invalid choice, please try again. Previous "
					        "choice: "
					     << choices << endl;
					break;
				}
				cout << "Bonus color chosen by the RandomPlayer: "
				     << toString(b) << endl;
			}
		} catch (SplendorException &e) {
			cout << e.getInfo() << "\n";
		}
	}
}

void RandomPlayer::applyRoyalCapacity(const RoyalCard &card,
                                      StrategyPlayer &opponent) {
	if (card.getCapacity().has_value()) {
		cout << "Applying the royal card's capacity!" << endl;
		std::optional<Capacity> capa = card.getCapacity();
		if (capa == Capacity::steal_opponent_pawn) {
			if (capa == Capacity::steal_opponent_pawn) {
				cout << "The RandomPlayer can steal a token!\n";
				if (opponent.getToken().empty()) {
					cout << "Too bad, you don't have any gem or perl token!"
					     << endl;
				} else {
					vector<const Token *> opponent_tokens =
					    opponent.getToken();
					int choice;
					do {
						choice = rand() % opponent_tokens.size();
					} while (opponent_tokens[choice]->getColor() ==
					         Color::gold);
					tokens.push_back(opponent_tokens[choice]);
					token_number++;
					opponent_tokens.erase(opponent_tokens.begin() + choice);
					opponent.setNbTokens(opponent.getTokenNumber() - 1);
				}
			}
		} else if (capa == Capacity::take_priviledge) {
			cout << "The RandomPlayer takes a privilege" << endl;
			Game::getGame().getCurrentPlayer().obtainPrivilege();
		} else {
			cout << "The RandomPlayer can replay" << endl;
			Game::getGame().nextRound(1);
		}
	}
}

void RandomPlayer::tokenVerification() {
	if (token_number > 10) {
		int nb = token_number - 10;
		cout << "The RandomPlayer has too many tokens, it must return " << nb
		     << " to the bag." << endl;
		cout << "Here are its tokens:" << endl;
		for (int i = 0; i < token_number; i++) {
			cout << "Index " << i << " : " << *tokens[i] << endl;
		}
		vector<int> tab(0);
		int i = 0;
		int curNb = nb;
		while (curNb > 0) {
			int tmp = rand() % token_number;
			// cout<<tmp<<endl;
			bool inTab = 0;
			for (int j = 0; j < tab.size();
			     j++) { // check if we haven't already tried to remove it
				if (tab[j] == tmp) {
					inTab = 1;
					cout << "Already in tab" << endl;
				}
			}
			if (tmp >= 0 and tmp < token_number and inTab == 0) {
				tab.push_back(tmp);
				curNb--;
			}
		}
		// sort the array
		std::make_heap(tab.begin(), tab.end());
		std::sort_heap(tab.begin(), tab.end());
		for (int k = tab.size() - 1; k >= 0;
		     k--) { // remove from end to start to avoid shifting
			        // indices
			Bag::get().insertToken(tokens[tab[k]]);
			tokens.erase(tokens.begin() + tab[k]);
			token_number--;
		}
	}
}

/******************** RandomPlayer ********************/
