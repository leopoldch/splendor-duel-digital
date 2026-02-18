//
// Created by leo on 18/12/2023.
//

#include "history.h"
History::Handler History::handler;

StrategyPlayer &fromJsonStrategyPLayer(json data) {
	if (data["is_ia"] == 1) {
		string tmp = data["name"];
		if (History::getHistory().inHistory(tmp, 1)) {
			StrategyPlayer *player = History::getHistory().pullPlayer(tmp, 1);
			player->setCrownNumber(data["crown_number"]);
			player->setPoints(data["points_number"]);
			player->setPrivilegeNumber(data["privilege_number"]);
			player->setRandomPlayer(data["is_ia"]);
			player->setNbTokens(data["token_number"]);
			player->setNbRCards(data["royal_cards_number"]);
			player->setNbJCards(data["jewelry_cards_number"]);
			player->setNbJCardsReserved(data["reserved_jewelry_cards_number"]);

			player->setTokens(tokensFromJson(data["tokens"]));
			player->setJewellryCard(
			    jewelryCardFromJson(data["bought_jewelry_cards"]));
			player->setJewellryCardReserved(
			    jewelryCardFromJson(data["reserved_jewelry_cards"]));
			player->setRoyalCard(royalCardsFromJson(data["royal_cards"]));
			player->setPrivileges(privilegesFromJson(data["privileges"],
			                                         data["privilege_number"]));
			return *player;
		} else {
			RandomPlayer *player = new RandomPlayer(tmp);
			player->setCrownNumber(data["crown_number"]);
			player->setPoints(data["points_number"]);
			player->setPrivilegeNumber(data["privilege_number"]);
			player->setRandomPlayer(data["is_ia"]);
			player->setNbTokens(data["token_number"]);
			player->setNbRCards(data["royal_cards_number"]);
			player->setNbJCards(data["jewelry_cards_number"]);
			player->setNbJCardsReserved(data["reserved_jewelry_cards_number"]);

			player->setTokens(tokensFromJson(data["tokens"]));
			player->setJewellryCard(
			    jewelryCardFromJson(data["bought_jewelry_cards"]));
			player->setJewellryCardReserved(
			    jewelryCardFromJson(data["reserved_jewelry_cards"]));
			player->setRoyalCard(royalCardsFromJson(data["royal_cards"]));
			player->setPrivileges(privilegesFromJson(data["privileges"],
			                                         data["privilege_number"]));
			return *player;
		}

	} else {
		string tmp = data["name"];
		if (History::getHistory().inHistory(tmp, 0)) {
			StrategyPlayer *player = History::getHistory().pullPlayer(tmp, 0);
			player->setCrownNumber(data["crown_number"]);
			player->setPoints(data["points_number"]);
			player->setPrivilegeNumber(data["privilege_number"]);
			player->setRandomPlayer(data["is_ia"]);
			player->setNbTokens(data["token_number"]);
			player->setNbRCards(data["royal_cards_number"]);
			player->setNbJCards(data["jewelry_cards_number"]);
			player->setNbJCardsReserved(data["reserved_jewelry_cards_number"]);

			player->setTokens(tokensFromJson(data["tokens"]));
			player->setJewellryCard(
			    jewelryCardFromJson(data["bought_jewelry_cards"]));
			player->setJewellryCardReserved(
			    jewelryCardFromJson(data["reserved_jewelry_cards"]));
			player->setRoyalCard(royalCardsFromJson(data["royal_cards"]));
			player->setPrivileges(privilegesFromJson(data["privileges"],
			                                         data["privilege_number"]));
			return *player;
		} else {
			Player *player = new Player(tmp);
			player->setCrownNumber(data["crown_number"]);
			player->setPoints(data["points_number"]);
			player->setPrivilegeNumber(data["privilege_number"]);
			player->setRandomPlayer(data["is_ia"]);
			player->setNbTokens(data["token_number"]);
			player->setNbRCards(data["royal_cards_number"]);
			player->setNbJCards(data["jewelry_cards_number"]);
			player->setNbJCardsReserved(data["reserved_jewelry_cards_number"]);

			player->setTokens(tokensFromJson(data["tokens"]));
			player->setJewellryCard(
			    jewelryCardFromJson(data["bought_jewelry_cards"]));
			player->setJewellryCardReserved(
			    jewelryCardFromJson(data["reserved_jewelry_cards"]));
			player->setRoyalCard(royalCardsFromJson(data["royal_cards"]));
			player->setPrivileges(privilegesFromJson(data["privileges"],
			                                         data["privilege_number"]));
			return *player;
		}
	}
}

Game::Game(json data) {
	Token::resetCounters();

	is_finished = data["is_finished"];
	cout << "init j1\n" << endl;

	who_plays = &fromJsonStrategyPLayer(data["who_plays"]);

	round = data["round"];
	cout << "init j2\n" << endl;
	opponent = &fromJsonStrategyPLayer(data["opponent"]);

	// voluntary not to init
	// jewelry_cards = jewelryCardFromJson(data["jewelry_cards"]);
	// tokens = tokensFromJson(data["tokens"]);

	royal_cards = royalCardsFromJson(data["royal_cards"]);
	privileges =
	    privilegesFromJson(data["privileges"], data["privilege_number"]);

	cout << "init deck1\n" << endl;
	deck1 = &deckFromJson(data["deck1"]);
	cout << "init deck2\n" << endl;
	deck2 = &deckFromJson(data["deck2"]);
	cout << "init deck3\n" << endl;
	deck3 = &deckFromJson(data["deck3"]);
	cout << "init draw1\n" << endl;
	draw1 = &drawFromJson(data["draw1"], *deck1);
	cout << "init draw2\n" << endl;
	draw2 = &drawFromJson(data["draw2"], *deck2);
	cout << "init draw3\n" << endl;
	draw3 = &drawFromJson(data["draw3"], *deck3);
	cout << "init bag\n" << endl;
	bagFromJson(data["bag"]);
	cout << "init board\n" << endl;
	boardFromJson(data["board"]);

	// Collect all token pointers for ownership (so ~Game deletes them)
	for (const auto* tok : who_plays->getToken()) {
		if (tok) tokens.push_back(tok);
	}
	for (const auto* tok : opponent->getToken()) {
		if (tok) tokens.push_back(tok);
	}
	for (int i = 0; i < Token::getMaxTokenNumber(); i++) {
		const Token* tok = Board::getBoard().getBoardCaseByIndex(i);
		if (tok) tokens.push_back(tok);
	}
	for (int i = 0; i < Bag::get().getTokenNumber(); i++) {
		tokens.push_back(Bag::get().getTokenByIndex(i));
	}
}

Match::Match(json data) {

	if (data["winner"]["is_ia"] == 1) {

		string tmp = data["winner"]["name"];
		if (!History::getHistory().inHistory(tmp, 1)) {
			winner = new RandomPlayer(tmp);
			winner->setWins(data["winner"]["games_won"]);
			winner->setPlayed(data["winner"]["games"]);
			winner->setRandomPlayer(1);
			History::getHistory().addPlayer();

		} else {
			winner = History::getHistory().pullPlayer(tmp, 1);
		}

	} else {
		string tmp = data["winner"]["name"];
		if (!History::getHistory().inHistory(tmp, 0)) {
			winner = new Player(tmp);
			winner->setWins(data["winner"]["games_won"]);
			winner->setPlayed(data["winner"]["games"]);
			winner->setRandomPlayer(0);
			History::getHistory().addPlayer();

		}

		else {
			winner = History::getHistory().pullPlayer(tmp, 0);
		}
	}
	if (data["opponent"]["is_ia"] == 1) {
		string tmp = data["opponent"]["name"];
		if (!History::getHistory().inHistory(tmp, 1)) {
			opponent = new RandomPlayer(tmp);
			opponent->setWins(data["opponent"]["games_won"]);
			opponent->setPlayed(data["opponent"]["games"]);
			opponent->setRandomPlayer(1);
			History::getHistory().addPlayer();
		} else {
			opponent = History::getHistory().pullPlayer(tmp, 1);
		}

	} else {
		string tmp = data["opponent"]["name"];
		if (!History::getHistory().inHistory(tmp, 0)) {
			opponent = new Player(tmp);
			opponent->setWins(data["opponent"]["games_won"]);
			opponent->setPlayed(data["opponent"]["games"]);
			opponent->setRandomPlayer(0);
			History::getHistory().addPlayer();
		} else {
			opponent = History::getHistory().pullPlayer(tmp, 0);
		}
	}
	winner_score = data["winner_score"];
	opponent_score = data["opponent_score"];
}

void Hist() {

	json j1; // retrieve the old history
	int tmp = 0;

	// add the new one
	json jtmp = History::getHistory().toHistory();
	cout << History::getHistory().getSize() << endl;

	for (int i = 0; i < History::getHistory().getSize(); ++i) {
		j1["matches"].push_back(jtmp[i]);
	}

	json j2 = {};
	j2["winner"] = Game::getGame().getCurrentPlayer().toHistory();
	j2["opponent"] = Game::getGame().getOpponent().toHistory();
	j2["winner_score"] = Game::getGame().getCurrentPlayer().getNbPoints();
	j2["opponent_score"] = Game::getGame().getOpponent().getNbPoints();
	j1["matches"].push_back(j2);

	tmp += History::getHistory().getSize();
	tmp += 1;

	j1["nb_matches"] = tmp;
	std::string history = j1.dump(2);
	cout << history << endl;
	std::ofstream file("../src/history.json");
	file << history;
}

void Game::setPlayers(string &name1, string &name2, string &user_player_choice1,
                      string &user_player_choice2) {

	if (name1 == name2) {
		throw SplendorException("Players must be different!");
	}
	if (name1 == "" or name2 == "") {
		throw SplendorException("Both players must have a name!");
	}

	if (user_player_choice1 == "RandomPlayer")
		user_player_choice1 = "I";
	else
		user_player_choice1 = "J";

	if (user_player_choice2 == "RandomPlayer")
		user_player_choice2 = "I";
	else
		user_player_choice2 = "J";

	srand(static_cast<unsigned>(std::time(nullptr)));
	if (rand() % 2 == 0) { // player who starts the game is randomly chosen
		if (user_player_choice1 == "J") {
			if (History::getHistory().inHistory(name1, 0)) {
				who_plays = History::getHistory().pullPlayer(name1, 0);
			} else {
				who_plays = new Player(name1);
				who_plays->setRandomPlayer(0);
			}
		} else {

			if (History::getHistory().inHistory(name1, 1)) {
				who_plays = History::getHistory().pullPlayer(name1, 1);
			} else {
				who_plays = new RandomPlayer(name1);
				who_plays->setRandomPlayer(1);
			}
		}

		if (user_player_choice2 == "J") {

			if (History::getHistory().inHistory(name2, 0)) {
				opponent = History::getHistory().pullPlayer(name2, 0);
			} else {
				opponent = new Player(name2);
				opponent->setRandomPlayer(0);
			}

		} else {
			if (History::getHistory().inHistory(name2, 1)) {
				opponent = History::getHistory().pullPlayer(name2, 1);
			} else {
				opponent = new RandomPlayer(name2);
				opponent->setRandomPlayer(1);
			}
		}
	} else {
		if (user_player_choice1 == "J") {
			if (History::getHistory().inHistory(name1, 0)) {
				opponent = History::getHistory().pullPlayer(name1, 0);
			} else {
				opponent = new Player(name1);
				opponent->setRandomPlayer(0);
			}

		} else {
			if (History::getHistory().inHistory(name1, 1)) {
				opponent = History::getHistory().pullPlayer(name1, 1);
			} else {
				opponent = new RandomPlayer(name1);
				opponent->setRandomPlayer(1);
			}
		}
		if (user_player_choice2 == "J") {

			if (History::getHistory().inHistory(name2, 0)) {
				who_plays = History::getHistory().pullPlayer(name2, 0);
			} else {
				who_plays = new Player(name2);
				who_plays->setRandomPlayer(0);
			}
		} else {
			if (History::getHistory().inHistory(name2, 1)) {
				who_plays = History::getHistory().pullPlayer(name2, 1);
			} else {
				who_plays = new RandomPlayer(name2);
				who_plays->setRandomPlayer(1);
			}
		}
	}
	opponent->obtainPrivilege(); // The player who doesn't start begins with
	                             // a privilege
}
