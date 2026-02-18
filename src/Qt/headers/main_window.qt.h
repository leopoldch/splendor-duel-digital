#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "board.qt.h"
#include "choice_popup.qt.h"
#include "draws.qt.h"
#include "tokens.h"
#include <QApplication>
#include <QDesktopServices>
#include <QDialog>
#include <QInputDialog>
#include <QLCDNumber>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

#include "deck_or_draw_popup.qt.h"
#include "history.h"
#include "info_popup.qt.h"
#include "text_popup.qt.h"
#include "token_view.qt.h"
#include "yesno_popup.qt.h"


class MainWindow : public QMainWindow {
	Q_OBJECT

  private:
	QLabel *topRoyal1;
	QLabel *topRoyal2;

	QLabel *bottomRoyal1;
	QLabel *bottomRoyal2;

	QLabel *topPrivileges;
	QLabel *bottomPrivileges;

	QLCDNumber *topScoreDisplay;
	QLCDNumber *bottomScoreDisplay;

	QPushButton *viewCardsButtonBottom;
	QPushButton *viewTokensButtonBottom;
	QPushButton *viewReservedCardsButtonBottom;

	QPushButton *viewCardsButtonTop;
	QPushButton *viewTokensButtonTop;
	QPushButton *viewReservedCardsButtonTop;

	QLabel *topPlayerNameLabel;
	QLabel *bottomPlayerNameLabel;

	QEventLoop wait_for_action_token;
	QEventLoop wait_for_action_card;

	Qt_Board *board;
	Qt_Draws *draws;

	QLabel *whoPlays;

	Game *game;

	bool buyingCard;
	bool stealingToken;

	struct Handler {
		MainWindow *instance = nullptr;
		~Handler() {
			delete instance;
			instance = nullptr;
		}
	};

	static Handler handler;
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	MainWindow(const MainWindow &) = delete;
	MainWindow &operator=(const MainWindow &) = delete;

	int token_click_index;
	Qt_card *last_card_click;

	bool discarding;

	QDialog *current_dialog;

	void closeEvent(QCloseEvent *event) override {
		Game::free();
		History::freeHistory();
		exit(0);
	}

  public:
	const bool getDiscarding() const { return discarding; }
	void setDiscarding(bool x) { discarding = x; }

	void setBuyingCard(bool x) { buyingCard = x; }
	const bool getBuyingCard() { return buyingCard; }

	void setStealingToken(bool x) { stealingToken = x; }
	const bool getStealingToken() { return stealingToken; }

	void updateWhoPlays();

	QEventLoop *getCardWaitLoop() { return &wait_for_action_card; }
	QEventLoop *getTokenWaitLoop() { return &wait_for_action_token; }

	int getTokenIndexOnClick() const { return token_click_index; }
	void setTokenClickIndex(int x) { token_click_index = x; }

	Qt_card *getLastCardClicked() const { return last_card_click; }
	void setLastCardClick(Qt_card *c) { last_card_click = c; }

	void updateTopScore(int score); // Method to update top score
	void updateBottomScore(int score); // -- bottom score

	void updateScores() {
		int s1 = Game::getGame().getCurrentPlayer().getNbPoints();
		bottomScoreDisplay->display(s1);
		int s2 = Game::getGame().getOpponent().getNbPoints();
		topScoreDisplay->display(s2);
	}

	void askNames() {

		InputPopup *popup = new InputPopup(this);
		popup->setModal(true);
		popup->exec();
	}

	void setTopPlayerName(const QString &name);
	void setBottomPlayerName(const QString &name);

	void updateBoard();
	void updateDraws();
	void updatePrivileges();

	static MainWindow &getMainWindow() {
		if (handler.instance == nullptr)
			handler.instance = new MainWindow();
		return *handler.instance;
	}

	void freeMainWindow() {
		delete handler.instance;
		handler.instance = nullptr;
	}

	void deactivateButtons();
	void activateTokens();
	void activateForReserve();
	void activateForBuy();
	void activateForRoyalCard();

	Qt_Draws *getDraws() const { return draws; }

	void activateTokenColor(const Color &c);

	void acceptCurrentDialog() {
		if (current_dialog != nullptr) {
			current_dialog->accept();
			current_dialog = nullptr;
		}
	}

	void setCurrentDialog(QDialog *d) { current_dialog = d; }

  private slots:
	void showBoughtCardsTop();
	void showReservedCardsTop();
	void showTokensTop();

	void showBoughtCardsBottom();
	void showReservedCardsBottom();
	void showTokensBottom();

	void YesNo(char *choice, const std::string &string);
	void fillBoard();
	void openWebLink();
	void nextAction(int *tmp, Player *j);
	void showInfo(const string &string);
	void colorChoice(Color *c, int *nb);
	void colorJoker(colorBonus *b);
	void showStats();
	void showStatsPlayers();

  public slots:
	void tokenClicked(Qt_token *);
	void cardClicked(Qt_card *);

  signals:
	void triggerNextAction(int *tmp, Player *j);
	void triggerYesNo(char *choice, const std::string &string = "");
	void triggerInfo(const string &string);
	void triggercolorChoice(Color *c, int *nb);
	void triggercolorJoker(colorBonus *b);
	void tokenActionDone();
	void cardActionDone();
};

#endif // MAINWINDOW_H
