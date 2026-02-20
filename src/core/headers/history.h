//
// Created by leo on 18/12/2023.
//

#ifndef LO21_SPLENDOR_DUEL_HISTORY_H
#define LO21_SPLENDOR_DUEL_HISTORY_H

#include "game.h"

class Match {

  private:
	StrategyPlayer *winner;
	StrategyPlayer *opponent;
	unsigned int winner_score;
	unsigned int opponent_score;

  public:
	json toHistory() {
		json j;
		j["winner"] = winner->toHistory();
		j["opponent"] = opponent->toHistory();
		j["winner_score"] = winner_score;
		j["opponent_score"] = opponent_score;
		return j;
	}

	Match(json data);

	~Match() {
		delete winner;
		delete opponent;
	}

	StrategyPlayer *getWinner() const { return winner; }
	StrategyPlayer *getOpponent() const { return opponent; }
	unsigned int getScoreWinner() const { return winner_score; }
	unsigned int getScoreOpponent() const { return opponent_score; }
};

class History {

  private:
	// singleton
	struct Handler {
		History *instance = nullptr;

		~Handler() {
			delete instance;
			instance = nullptr;
		}
	};

	static Handler handler;

	// attributes
	vector<Match *> matches;
	unsigned int nb_match = 0;
	unsigned int nb_players = 0;

	// methods

	History() = default;

	~History() {
		for (Match *match : matches) {
			delete match;
		}
		matches.clear();
	}

	History(const History &) = delete;

	History &operator=(const History &) = delete;

  public:
	static void freeHistory() { delete &History::getHistory(); }

	void addPlayer() { nb_players++; }

	const unsigned int getNbPlayers() const { return nb_players; }

	vector<Match *> getMatches() const { return matches; }

	const bool inHistory(const string name, const int is_ia) const {

		for (int i = 0; i < matches.size(); ++i) {
			if (matches[i]->getWinner()->getName() == name &&
			    matches[i]->getWinner()->getRandomPlayer() == is_ia)
				return true;
			if (matches[i]->getOpponent()->getName() == name &&
			    matches[i]->getOpponent()->getRandomPlayer() == is_ia)
				return true;
		}
		return false;
	}

	unsigned int getSize() const { return nb_match; }

	void initHistory(json data) {
		for (int i = 0; i < data["nb_matches"]; ++i) {
			Match *tmp = new Match(data["matches"][i]);
			matches.push_back(tmp);
			nb_match++;
		}
	}

	StrategyPlayer *pullPlayer(const string name, const unsigned int is_ia) {
		for (int i = 0; i < matches.size(); ++i) {
			if (matches[i]->getWinner()->getName() == name &&
			    matches[i]->getWinner()->getRandomPlayer() == is_ia) {
				return matches[i]->getWinner();
			}
			if (matches[i]->getOpponent()->getName() == name &&
			    matches[i]->getOpponent()->getRandomPlayer() == is_ia) {
				return matches[i]->getOpponent();
			}
		}
		throw SplendorException("Player not found in history");
	}

	json toHistory() {

		json j;
		j = {};
		for (int i = 0; i < matches.size(); ++i) {
			j.push_back(matches[i]->toHistory());
		}
		return j;
	}

	static History &getHistory() {
		if (handler.instance == nullptr)
			handler.instance = new History;
		return *handler.instance;
	}
};

void Hist();

#endif // LO21_SPLENDOR_DUEL_HISTORY_H
