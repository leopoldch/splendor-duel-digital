#ifndef MAIN_H
#define MAIN_H

#ifndef LO21_SPLENDOR_DUEL_MAIN_H
#define LO21_SPLENDOR_DUEL_MAIN_H

#include "history.h"
#include "main_window.qt.h"
#include <QApplication>
#include <QThread>
#include <iostream>

void toJson() {
	json j = Game::getGame().toJson();
	std::string s = j.dump(2);
	std::ofstream file("../src/backup.json");
	file << s;
}

void tryOpenHistoryFile(){
	const std::string PATH = "../src/history.json";
	try {
		std::ifstream file(PATH);

		if (!file.is_open()) {
			std::cerr << "Failed to open the JSON file." << std::endl;
			throw SplendorException("Could not open history.json");
		}
		json hist;
		file >> hist;
		file.close();
		History::getHistory().initHistory(hist);
	} catch (SplendorException &e) {
		cout << "Couldn't open file, check if the file is present." << endl;
	}
}

void gameFromScratch(int argc, char *argv[]) {

	srand(static_cast<unsigned>(std::time(nullptr)));

	tryOpenHistoryFile();

	// Init the game
	Game::getGame();

	MainWindow::getMainWindow().show();
	Qt_Board::getBoard().connectTokens();
	MainWindow::getMainWindow().getDraws()->connectCards();

	// Setup names

	bool check_names = true;
	while (check_names) {
		try {
			MainWindow::getMainWindow().askNames();
			check_names = false;
		} catch (SplendorException &e) {
			MainWindow::getMainWindow().triggerInfo(e.getInfo());
		}
	}

	MainWindow::getMainWindow().setTopPlayerName(
	    QString::fromStdString(Game::getGame().getCurrentPlayer().getName()));
	MainWindow::getMainWindow().setBottomPlayerName(
	    QString::fromStdString(Game::getGame().getOpponent().getName()));

	// Setup the board
	Game::getGame().getPlayerRound();
	MainWindow::getMainWindow().updateBoard();
	MainWindow::getMainWindow().updateDraws();
	MainWindow::getMainWindow().updatePrivileges();

	unsigned int from_error = 0;

	while (!Game::getGame().isFinished()) {

		if (from_error == 0) {
			Game::getGame().getPlayerRound();
			MainWindow::getMainWindow().setTopPlayerName(QString::fromStdString(
			    Game::getGame().getOpponent().getName()));
			MainWindow::getMainWindow().setBottomPlayerName(
			    QString::fromStdString(
			        Game::getGame().getCurrentPlayer().getName()));
			// qDebug() << Game::getGame().getCurrentPlayer().getName();
			MainWindow::getMainWindow().updateWhoPlays();

			try {
				toJson();
			} catch (SplendorException &e) {
				cout << e.getInfo() << endl;
				MainWindow::getMainWindow().triggerInfo(e.getInfo());
			}
		}
		try {
			// qDebug() << "DEBUT TRY";
			Game::getGame().getCurrentPlayer().choice_Qt();
			// update display

			Game::getGame().getCurrentPlayer().tokenVerification_Qt();

			if (Game::getGame().getCurrentPlayer().royalCardEligibility() ==
			    1) {
				Game::getGame().getCurrentPlayer().royalCardSelection_Qt();
			}

			Game::getGame().nextRound();

			MainWindow::getMainWindow().updateScores();
			MainWindow::getMainWindow().updateBoard();
			MainWindow::getMainWindow().updateDraws();
			MainWindow::getMainWindow().updatePrivileges();
			MainWindow::getMainWindow().update();

			QCoreApplication::processEvents();

			from_error = 0;

		} catch (SplendorException &e) {
			from_error = 1;
			cout << "============= UNAUTHORIZED ACTION ================="
			     << endl;
			cout << e.getInfo() << endl;
		}
	}

	MainWindow::getMainWindow().updateScores();
	MainWindow::getMainWindow().updateBoard();
	MainWindow::getMainWindow().updateDraws();
	MainWindow::getMainWindow().updatePrivileges();
	MainWindow::getMainWindow().update();

	cout << "=================== Game over ===================" << endl;
	cout << "Number of rounds: " << Game::getGame().getRoundCount() << endl;
	cout << "Winner stats:" << endl;
	Game::getGame().getCurrentPlayer().printPlayer();
	Game::getGame().getCurrentPlayer().gameEnded(1);
	Game::getGame().getOpponent().gameEnded(0);

	cout << "Loser stats: " << endl;
	Game::getGame().getOpponent().printPlayer();

	try {
		Hist();
		toJson();
	} catch (SplendorException &e) {
		cout << e.getInfo() << endl;
	}

	MainWindow::getMainWindow().triggerInfo(
	    "Bravo, " + Game::getGame().getCurrentPlayer().getName() + " won!");
}

void gameFromJson(int argc, char *argv[]) {

	tryOpenHistoryFile();

	try {
		std::ifstream file("../src/backup.json");

		if (!file.is_open()) {
			std::cerr << "Failed to open the JSON file." << std::endl;
			throw SplendorException("Could not open backup.json");
		}

		json data;
		file >> data;
		file.close();

		// if maximum cards reached then too many cards generated.
		// bad card management!

		if (data["is_finished"])
			throw SplendorException("This game is over!");

		Game::getGame(data);

		cout << "player cards: " << endl;
		cout << Game::getGame().getCurrentPlayer().getBoughtCardNumber()
		     << endl;
		cout << Game::getGame().getOpponent().getBoughtCardNumber() << endl;

		cout << "draw cards: " << endl;

		cout << Game::getGame().getFirstDraw()->getCardsNumber() << endl
		     << endl;
		cout << Game::getGame().getSecondDraw()->getCardsNumber() << endl
		     << endl;
		cout << Game::getGame().getThirdDraw()->getCardsNumber() << endl
		     << endl;

		cout << "royal cards: " << endl;
		cout << Game::getGame().getRoyalCards().size() << endl;

		cout << "Player types: " << endl;
		cout << Game::getGame().getOpponent().getRandomPlayer() << endl;
		cout << Game::getGame().getCurrentPlayer().getRandomPlayer() << endl;

	} catch (SplendorException &e) {
		cout << e.getInfo() << endl;
		MainWindow::getMainWindow().triggerInfo(
		    e.getInfo() + "\nYou will start a new game from scratch:");
		gameFromScratch(argc, argv);
	}

	srand(static_cast<unsigned>(std::time(nullptr)));

	MainWindow::getMainWindow().show();
	Qt_Board::getBoard().connectTokens();
	MainWindow::getMainWindow().getDraws()->connectCards();

	// Setup names
	MainWindow::getMainWindow().setTopPlayerName(
	    QString::fromStdString(Game::getGame().getCurrentPlayer().getName()));
	MainWindow::getMainWindow().setBottomPlayerName(
	    QString::fromStdString(Game::getGame().getOpponent().getName()));

	// Setup the board
	Game::getGame().getPlayerRound();
	MainWindow::getMainWindow().updateBoard();
	MainWindow::getMainWindow().updateDraws();
	MainWindow::getMainWindow().updatePrivileges();

	cout << "player cards: " << endl;
	cout << Game::getGame().getCurrentPlayer().getBoughtCardNumber() << endl;
	cout << Game::getGame().getOpponent().getBoughtCardNumber() << endl;

	cout << "draw cards: " << endl;

	cout << Game::getGame().getFirstDraw()->getCardsNumber() << endl << endl;
	cout << Game::getGame().getSecondDraw()->getCardsNumber() << endl << endl;
	cout << Game::getGame().getThirdDraw()->getCardsNumber() << endl << endl;

	unsigned int from_error = 0;

	while (!Game::getGame().isFinished()) {

		if (from_error == 0) {
			Game::getGame().getPlayerRound();
			MainWindow::getMainWindow().setTopPlayerName(QString::fromStdString(
			    Game::getGame().getOpponent().getName()));
			MainWindow::getMainWindow().setBottomPlayerName(
			    QString::fromStdString(
			        Game::getGame().getCurrentPlayer().getName()));
			MainWindow::getMainWindow().updateWhoPlays();

			try {
				toJson();
			} catch (SplendorException &e) {
				cout << e.getInfo() << endl;
				MainWindow::getMainWindow().triggerInfo(e.getInfo());
			}
		}
		try {
			Game::getGame().getCurrentPlayer().choice_Qt();

			Game::getGame().getCurrentPlayer().tokenVerification_Qt();

			if (Game::getGame().getCurrentPlayer().royalCardEligibility() ==
			    1) {
				Game::getGame().getCurrentPlayer().royalCardSelection_Qt();
			}

			Game::getGame().nextRound();

			MainWindow::getMainWindow().updateScores();
			MainWindow::getMainWindow().updateBoard();
			MainWindow::getMainWindow().updateDraws();
			MainWindow::getMainWindow().updatePrivileges();
			MainWindow::getMainWindow().update();

			QCoreApplication::processEvents();

			from_error = 0;

		} catch (SplendorException &e) {
			from_error = 1;
			cout << "============= UNAUTHORIZED ACTION ================="
			     << endl;
			cout << e.getInfo() << endl;
		}
	}

	cout << "=================== Game over ===================" << endl;
	cout << "Number of rounds: " << Game::getGame().getRoundCount() << endl;
	cout << "Winner stats:" << endl;
	Game::getGame().getCurrentPlayer().printPlayer();
	Game::getGame().getCurrentPlayer().gameEnded(1);
	Game::getGame().getOpponent().gameEnded(0);

	try {
		Hist();
		toJson();
	} catch (SplendorException &e) {
		cout << e.getInfo() << endl;
	}
	MainWindow::getMainWindow().triggerInfo(
	    "Bravo, " + Game::getGame().getCurrentPlayer().getName() + " won!");
}

#endif // LO21_SPLENDOR_DUEL_MAIN_H

#endif // MAIN_H
