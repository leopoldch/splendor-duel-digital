#include "main_window.qt.h"
#include "color_popup.qt.h"
#include "history.h"
#include "joker_popup.qt.h"
#include <QDialog>
#include <QScrollArea>
#include <math.h>
#include <sstream>

MainWindow::Handler MainWindow::handler;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), buyingCard(false), current_dialog(nullptr) {
	QLabel *label = new QLabel(this);
	QPixmap pixmap("/path/to/your/image.png");
	label->setPixmap(pixmap);
	label->show();

	topRoyal1 = new QLabel(this);
	topRoyal1->setStyleSheet("background: transparent;");
	topRoyal1->setFixedSize(50, 80);
	topRoyal1->setPixmap(QPixmap());
	topRoyal2 = new QLabel(this);
	topRoyal2->setStyleSheet("background: transparent;");
	topRoyal2->setFixedSize(50, 80);
	topRoyal2->setPixmap(QPixmap());

	bottomRoyal1 = new QLabel(this);
	bottomRoyal1->setStyleSheet("background: transparent;");
	bottomRoyal1->setFixedSize(50, 80);
	bottomRoyal1->setPixmap(QPixmap());
	bottomRoyal2 = new QLabel(this);
	bottomRoyal2->setStyleSheet("background: transparent;");
	bottomRoyal2->setFixedSize(50, 80);
	bottomRoyal2->setPixmap(QPixmap());

	topPrivileges = new QLabel(this);
	topPrivileges->setStyleSheet("QLabel { color: white;}");
	bottomPrivileges = new QLabel(this);
	bottomPrivileges->setStyleSheet("QLabel { color: white;}");

	game = &Game::getGame();

	// Initialize player name labels
	topPlayerNameLabel = new QLabel("Player 1", this);
	bottomPlayerNameLabel = new QLabel("Player 2", this);
	topPlayerNameLabel->setStyleSheet(
	    "QLabel { color: white; background-color: "
	    "white; border: 1px solid white; }");
	bottomPlayerNameLabel->setStyleSheet(
	    "QLabel { color: white; background-color: white; border: 1px solid "
	    "white; }");

	QWidget *centralWidget = new QWidget(this);

	// Background for the entire window
	centralWidget->setStyleSheet(
	    "background-image: url('../src/assets/background.jpg'); background-position: "
	    "center;");

	QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
	mainLayout->setSpacing(16);

	whoPlays = new QLabel("Current player", this);
	whoPlays->setStyleSheet("QLabel { color: white; background-color: white; "
	                       "border: 1px solid white; }");
	mainLayout->addWidget(whoPlays, 0, Qt::AlignLeft);

	// ######## TOP SCORE AND BUTTONS ########

	// Scores on QLCD Display
	topScoreDisplay = new QLCDNumber(this);    // Display with 2 digits
	bottomScoreDisplay = new QLCDNumber(this); // Display with 2 digits

	// Display 0 for now
	topScoreDisplay->display(0);
	bottomScoreDisplay->display(0);

	// Add them to the main layout
	QGridLayout *topPlayer = new QGridLayout;
	topPlayer->addWidget(topPlayerNameLabel, 0, 1, Qt::AlignCenter);
	topPlayer->addWidget(topScoreDisplay, 1, 1, Qt::AlignCenter);

	QHBoxLayout *topRoyalLayout = new QHBoxLayout();
	topRoyalLayout->addWidget(topRoyal1);
	topRoyalLayout->addWidget(topRoyal2);
	topRoyalLayout->addWidget(topPrivileges);

	QWidget *royalWidget = new QWidget();
	royalWidget->setLayout(topRoyalLayout);

	topPlayer->addWidget(royalWidget, 0, 2, 2, 1, Qt::AlignRight);

	topPlayer->setColumnStretch(0, 1);
	topPlayer->setColumnStretch(1, 1);
	topPlayer->setColumnStretch(2, 1);

	mainLayout->addLayout(topPlayer);

	// Top buttons (cards and tokens)
	viewCardsButtonTop = new QPushButton("View bought cards", this);
	viewTokensButtonTop = new QPushButton("View tokens", this);
	viewReservedCardsButtonTop = new QPushButton("View reserved cards", this);

	// Make the text white
	viewCardsButtonTop->setStyleSheet("color: rgba(255, 255, 255, 255);");
	viewTokensButtonTop->setStyleSheet("color: rgba(255, 255, 255, 255);");
	viewReservedCardsButtonTop->setStyleSheet(
	    "color: rgba(255, 255, 255, 255);");

	// Connect to features
	connect(viewCardsButtonTop, &QPushButton::clicked, this,
	        &MainWindow::showBoughtCardsTop);
	connect(viewTokensButtonTop, &QPushButton::clicked, this,
	        &MainWindow::showTokensTop);
	connect(viewReservedCardsButtonTop, &QPushButton::clicked, this,
	        &MainWindow::showReservedCardsTop);

	// Add a layout and put them in it
	QHBoxLayout *topButtonLayout = new QHBoxLayout(this);
	topButtonLayout->addWidget(viewCardsButtonTop);
	topButtonLayout->addWidget(viewTokensButtonTop);
	topButtonLayout->addWidget(viewReservedCardsButtonTop);
	mainLayout->addLayout(topButtonLayout);

	// ######## BOARD AND DRAWS ########

	// Board and draws
	QWidget *middleContainer = new QWidget(this);
	QHBoxLayout *middleLayout = new QHBoxLayout(middleContainer);

	// Create the board and draws
	board = &Qt_Board::getBoard();
	draws = new Qt_Draws(this);

	QVBoxLayout *boardLayout = new QVBoxLayout(this);
	boardLayout->addWidget(board);

	middleLayout->addLayout(
	    boardLayout); // Add the board and button to the middle layout
	middleLayout->addStretch(); // Stretch to put the draws to the right
	draws->setStyleSheet("background-image: url('../src/assets/background.jpg'); "
	                       "background-position: center;");
	middleLayout->addWidget(draws);
	mainLayout->addWidget(middleContainer);

	// ######## VICTORY CONDITIONS AND RULES ########

	QVBoxLayout *rules = new QVBoxLayout;

	// Add rules button
	QPushButton *viewRules = new QPushButton("View rules", this);
	QPushButton *viewStats = new QPushButton("History", this);
	QPushButton *viewStatsPlayers = new QPushButton("Statistics", this);
	viewRules->setStyleSheet("color: rgba(255, 255, 255, 255);");
	viewRules->setFixedWidth(397 / 3);
	viewStats->setStyleSheet("color: rgba(255, 255, 255, 255);");
	viewStats->setFixedWidth(397 / 3);
	viewStatsPlayers->setStyleSheet("color: rgba(255, 255, 255, 255);");
	viewStatsPlayers->setFixedWidth(397 / 3);
	connect(viewRules, &QPushButton::clicked, this, &MainWindow::openWebLink);
	connect(viewStats, &QPushButton::clicked, this, &MainWindow::showStats);
	connect(viewStatsPlayers, &QPushButton::clicked, this,
	        &MainWindow::showStatsPlayers);

	// Victory conditions and its image
	QLabel *victoryConditions = new QLabel(this);
	QPixmap originalPixmap(
	    "../src/assets/rest_detoured/Conditions_victoire.png");
	QPixmap scaledPixmap = originalPixmap.scaled(
	    397 / 3, 330 / 3, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	victoryConditions->setPixmap(scaledPixmap);
	victoryConditions->setFixedSize(scaledPixmap.size());

	// Add them to the layout
	rules->addWidget(victoryConditions, Qt::AlignCenter);
	rules->addWidget(viewRules, Qt::AlignCenter);
	rules->addWidget(viewStats, Qt::AlignCenter);
	rules->addWidget(viewStatsPlayers, Qt::AlignCenter);
	rules->addStretch(1);

	middleLayout->addLayout(rules, Qt::AlignCenter);

	// ######## BOTTOM SCORE AND BUTTONS ########

	QGridLayout *bottomPlayer = new QGridLayout;
	bottomPlayer->addWidget(bottomPlayerNameLabel, 0, 1, Qt::AlignCenter);
	bottomPlayer->addWidget(bottomScoreDisplay, 1, 1, Qt::AlignCenter);

	QHBoxLayout *bottomRoyalLayout = new QHBoxLayout();
	bottomRoyalLayout->addWidget(bottomRoyal1);
	bottomRoyalLayout->addWidget(bottomRoyal2);
	bottomRoyalLayout->addWidget(bottomPrivileges);

	QWidget *royalWidgetBottom = new QWidget();
	royalWidgetBottom->setLayout(bottomRoyalLayout);

	bottomPlayer->addWidget(royalWidgetBottom, 0, 2, 2, 1, Qt::AlignRight);

	bottomPlayer->setColumnStretch(0, 1);
	bottomPlayer->setColumnStretch(1, 1);
	bottomPlayer->setColumnStretch(2, 1);

	mainLayout->addLayout(bottomPlayer);

	// Bottom buttons
	viewCardsButtonBottom = new QPushButton("View bought cards", this);
	viewTokensButtonBottom = new QPushButton("View tokens", this);
	viewReservedCardsButtonBottom =
	    new QPushButton("View reserved cards", this);

	viewCardsButtonBottom->setStyleSheet("color: rgba(255, 255, 255, 255);");
	viewTokensButtonBottom->setStyleSheet("color: rgba(255, 255, 255, 255);");
	viewReservedCardsButtonBottom->setStyleSheet(
	    "color: rgba(255, 255, 255, 255);");
	connect(viewCardsButtonBottom, &QPushButton::clicked, this,
	        &MainWindow::showBoughtCardsBottom);
	connect(viewTokensButtonBottom, &QPushButton::clicked, this,
	        &MainWindow::showTokensBottom);
	connect(viewReservedCardsButtonBottom, &QPushButton::clicked, this,
	        &MainWindow::showReservedCardsBottom);

	QHBoxLayout *bottomButtonLayout = new QHBoxLayout;
	bottomButtonLayout->addWidget(viewCardsButtonBottom);
	bottomButtonLayout->addWidget(viewTokensButtonBottom);
	bottomButtonLayout->addWidget(viewReservedCardsButtonBottom);
	mainLayout->addLayout(bottomButtonLayout);

	// Set the central widget
	setCentralWidget(centralWidget);

	// Connections
	connect(this, &MainWindow::triggerNextAction, this,
	        &MainWindow::nextAction);
	connect(this, &MainWindow::triggerYesNo, this, &MainWindow::YesNo);
	connect(this, &MainWindow::triggerInfo, this, &MainWindow::showInfo);
	connect(this, &MainWindow::triggercolorChoice, this,
	        &MainWindow::colorChoice);
	connect(this, &MainWindow::triggercolorJoker, this,
	        &MainWindow::colorJoker);
	connect(this, &MainWindow::tokenActionDone, &wait_for_action_token,
	        &QEventLoop::quit);
	connect(this, &MainWindow::cardActionDone, &wait_for_action_card,
	        &QEventLoop::quit);
}

MainWindow::~MainWindow() {
	// Nothing
}

void MainWindow::showTokensBottom() {
	QDialog *tokensDialog = new QDialog(this);
	setCurrentDialog(tokensDialog);
	tokensDialog->setStyleSheet(
	    "background-image: url('../src/assets/background.jpg'); "
	    "background-position: center;");

	QVBoxLayout *verticallayout = new QVBoxLayout(tokensDialog);

	QLabel *name = new QLabel(tokensDialog);
	name->setText(
	    "Tokens of " +
	    QString::fromStdString(Game::getGame().getCurrentPlayer().getName()));
	name->setStyleSheet("QLabel { color: white; }");

	verticallayout->addWidget(name, Qt::AlignCenter);

	QGridLayout *layout = new QGridLayout(tokensDialog);
	QWidget *gridWidget = new QWidget(tokensDialog);

	gridWidget->setLayout(layout);

	int nb = Game::getGame().getCurrentPlayer().getTokenNumber();

	for (int i = 0; i < 16; i++) {
		Qt_token *j = new Qt_token(tokensDialog);
		j->setIndice(i);
		if (i < nb)
			j->setToken(Game::getGame().getCurrentPlayer().getToken()[i]);
		j->setFixedSize(
		    60, 60); // Width: 100px, Height: 140px based on 1:1.4 aspect ratio
		j->setStyleSheet("border: 1px solid black;");
		layout->addWidget(j, i / 4, i % 4);
		if (getDiscarding() == true)
			connect(j, &Qt_token::tokenClicked, &MainWindow::getMainWindow(),
			        &MainWindow::tokenClicked);
		j->updateAppearance();
	}

	// Disable stretching and set fixed size for the layout
	for (int i = 0; i < 4; ++i) {
		layout->setRowStretch(i, 0);
		layout->setColumnStretch(i, 0);
	}

	verticallayout->addWidget(gridWidget);
	tokensDialog->setLayout(verticallayout);

	tokensDialog->show();
}

void MainWindow::showReservedCardsBottom() {
	QDialog *cardsDialog = new QDialog(this);
	cardsDialog->setStyleSheet(
	    "background-image: url('../src/assets/background.jpg'); "
	    "background-position: center;");
	QGridLayout *layout = new QGridLayout(cardsDialog);

	int nb = Game::getGame().getCurrentPlayer().getReservedCardNumber();
	for (int i = 0; i < nb; i++) {
		qDebug() << "RESERVED CARDS";
		Qt_card *c = new Qt_card(cardsDialog);
		c->setCard(Game::getGame().getCurrentPlayer().getReservedCards()[i]);
		c->setFixedSize(75, 105);
		if (getBuyingCard() == true)
			c->setDisabled(false);
		else
			c->setDisabled(true);
		c->setIndice(i);
		c->setReserved(true);
		connect(c, &Qt_card::cardClicked, &MainWindow::getMainWindow(),
		        &MainWindow::cardClicked);
		connect(c, &Qt_card::cardClicked, cardsDialog, &QDialog::accept);
		layout->addWidget(c, i / 4, i % 4);
		c->updateAppearance();
		qDebug() << c->getCard()->getVisual();
	}

	cardsDialog->setLayout(layout);
	cardsDialog->exec(); // Show it
}

void MainWindow::showBoughtCardsBottom() {
	QDialog *cardsDialog = new QDialog(this);
	cardsDialog->setStyleSheet(
	    "background-image: url('../src/assets/background.jpg'); "
	    "background-position: center;");
	QGridLayout *layout = new QGridLayout(cardsDialog);

	int nb = Game::getGame().getCurrentPlayer().getJewelryCardNumber();
	for (int i = 0; i < nb; i++) {
		qDebug() << "BOUGHT CARDS";
		Qt_card *c = new Qt_card(cardsDialog);
		c->setCard(Game::getGame().getCurrentPlayer().getBoughtCards()[i]);
		c->setFixedSize(75, 105);
		c->setDisabled(true);
		layout->addWidget(c, i / 4, i % 4);
		c->updateAppearance();
		qDebug() << c->getCard()->getVisual();
	}

	cardsDialog->setLayout(layout);
	cardsDialog->exec(); // Show it
}

void MainWindow::showBoughtCardsTop() {
	QDialog *cardsDialog = new QDialog(this);
	cardsDialog->setStyleSheet(
	    "background-image: url('../src/assets/background.jpg'); "
	    "background-position: center;");
	QGridLayout *layout = new QGridLayout(cardsDialog);

	int nb = Game::getGame().getOpponent().getJewelryCardNumber();
	for (int i = 0; i < nb; i++) {
		qDebug() << "BOUGHT CARDS";
		Qt_card *c = new Qt_card(cardsDialog);
		c->setCard(Game::getGame().getOpponent().getBoughtCards()[i]);
		c->setFixedSize(75, 105);
		c->setDisabled(true);
		layout->addWidget(c, i / 4, i % 4);
		c->updateAppearance();
		qDebug() << c->getCard()->getVisual();
	}

	cardsDialog->setLayout(layout);
	cardsDialog->exec(); // Show it
}

void MainWindow::showTokensTop() {
	QDialog *tokensDialog = new QDialog(this);
	tokensDialog->setStyleSheet(
	    "background-image: url('../src/assets/background.jpg'); "
	    "background-position: center;");
	setCurrentDialog(tokensDialog);

	QVBoxLayout *verticallayout = new QVBoxLayout(tokensDialog);

	QLabel *name = new QLabel(tokensDialog);
	name->setText("Tokens of " + QString::fromStdString(
	                                 Game::getGame().getOpponent().getName()));
	name->setStyleSheet("QLabel { color: white; }");

	verticallayout->addWidget(name, Qt::AlignCenter);

	QGridLayout *layout = new QGridLayout(tokensDialog);
	QWidget *gridWidget = new QWidget(tokensDialog);

	gridWidget->setLayout(layout);

	int nb = Game::getGame().getOpponent().getTokenNumber();

	for (int i = 0; i < 16; i++) {
		Qt_token *j = new Qt_token(tokensDialog);
		j->setIndice(i);
		if (i < nb)
			j->setToken(Game::getGame().getOpponent().getToken()[i]);
		j->setFixedSize(60, 60);
		j->setStyleSheet("border: 1px solid black;");
		layout->addWidget(j, i / 4, i % 4);
		j->setDisabled(true);
		if (i < nb && MainWindow::getMainWindow().getStealingToken() == true &&
		    Game::getGame().getOpponent().getToken()[i]->getColor() !=
		        Color::gold) {
			j->setDisabled(false);
			connect(j, &Qt_token::tokenClicked, &MainWindow::getMainWindow(),
			        &MainWindow::tokenClicked);
			connect(j, &Qt_token::tokenClicked, tokensDialog, &QDialog::accept);
		}
		j->updateAppearance();
	}

	// Disable stretching and set fixed size for the layout
	for (int i = 0; i < 4; ++i) {
		layout->setRowStretch(i, 0);
		layout->setColumnStretch(i, 0);
	}

	verticallayout->addWidget(gridWidget);
	tokensDialog->setLayout(verticallayout);

	tokensDialog->exec();
}

void MainWindow::showReservedCardsTop() {
	QDialog *cardsDialog = new QDialog(this);
	cardsDialog->setStyleSheet(
	    "background-image: url('../src/assets/background.jpg'); "
	    "background-position: center;");
	QGridLayout *layout = new QGridLayout(cardsDialog);

	int nb = Game::getGame().getOpponent().getReservedCardNumber();
	for (int i = 0; i < nb; i++) {
		qDebug() << "RESERVED CARDS";
		Qt_card *c = new Qt_card(cardsDialog);
		c->setCard(Game::getGame().getOpponent().getReservedCards()[i]);
		c->setFixedSize(75, 105);
		c->setDisabled(true);
		c->setIndice(i);
		layout->addWidget(c, i / 4, i % 4);
		c->updateAppearance();
		qDebug() << c->getCard()->getVisual();
	}

	cardsDialog->setLayout(layout);
	cardsDialog->exec(); // Show it
}

void MainWindow::updateTopScore(int score) { topScoreDisplay->display(score); }

void MainWindow::updateBottomScore(int score) {
	bottomScoreDisplay->display(score);
}

void MainWindow::fillBoard() {
	// Put the action here
}

void MainWindow::openWebLink() {
	QDesktopServices::openUrl(
	    QUrl("https://cdn.1j1ju.com/medias/da/39/6a-splendor-duel-regle.pdf"));
}

void MainWindow::setTopPlayerName(const QString &name) {
	topPlayerNameLabel->setText(name);
	topPlayerNameLabel->update();
}

void MainWindow::setBottomPlayerName(const QString &name) {
	bottomPlayerNameLabel->setText(name);
	bottomPlayerNameLabel->update();
}

void MainWindow::updateDraws() {

	// Update card pointers

	// Draw 1
	qDebug() << "Draw1";
	for (int i = 0; i < 5; i++) {
		draws->getTier1()[i]->setCard(
		    Game::getGame().getFirstDraw()->getDrawCards()[i]);

		// Update visuals
		if (i < Game::getGame().getFirstDraw()->getCardsNumber())
			draws->getTier1()[i]->updateAppearance();
		else {
			draws->getTier1()[i]->setIcon(QIcon());
			draws->getTier1()[i]->setIconSize(this->size());
		}
	}

	// Draw 2
	qDebug() << "Draw2";
	for (int i = 0; i < 4; i++) {
		draws->getTier2()[i]->setCard(
		    Game::getGame().getSecondDraw()->getDrawCards()[i]);

		// Update visuals
		if (i < Game::getGame().getSecondDraw()->getCardsNumber())
			draws->getTier2()[i]->updateAppearance();
		else {
			draws->getTier2()[i]->setIcon(QIcon());
			draws->getTier2()[i]->setIconSize(this->size());
		}
	}

	// Draw 3
	qDebug() << "Draw3";
	for (int i = 0; i < 3; i++) {
		draws->getTier3()[i]->setCard(
		    Game::getGame().getThirdDraw()->getDrawCards()[i]);

		// Update visuals
		if (i < Game::getGame().getThirdDraw()->getCardsNumber())
			draws->getTier3()[i]->updateAppearance();
		else {
			draws->getTier3()[i]->setIcon(QIcon());
			draws->getTier3()[i]->setIconSize(this->size());
		}
	}

	// Royal cards
	qDebug() << "RoyalCards";

	auto currentRoyalCards = Game::getGame().getRoyalCards(); 

	for (int i = 0; i < 4; i++) {
		if (i < currentRoyalCards.size()) {
			draws->getRoyalCards()[i]->setCard(currentRoyalCards[i]);
			draws->getRoyalCards()[i]->updateAppearance();
			draws->getRoyalCards()[i]->setEnabled(true);
		} 
		else {
			draws->getRoyalCards()[i]->setCard(nullptr);
			draws->getRoyalCards()[i]->setIcon(QIcon());
			draws->getRoyalCards()[i]->setIconSize(this->size());
			draws->getRoyalCards()[i]->setDisabled(true);
		}
	}

	if (draws->getDeck1() != nullptr) {
		if (!Game::getGame().getDeck(1)->isEmpty())
			draws->getDeck1()->updateAppearance(
			    "../src/assets/rest_detoured/Pioche_niveau_1.png");
		else {
			draws->getDeck1()->setIcon(QIcon());
			draws->getDeck1()->setIconSize(this->size());
			draws->getDeck1()->setDisabled(true);
		}
	}
	if (draws->getDeck2() != nullptr) {
		if (!Game::getGame().getDeck(2)->isEmpty())
			draws->getDeck2()->updateAppearance(
			    "../src/assets/rest_detoured/Pioche_niveau_2.png");
		else {
			draws->getDeck2()->setIcon(QIcon());
			draws->getDeck2()->setIconSize(this->size());
			draws->getDeck2()->setDisabled(true);
		}
	}
	if (draws->getDeck3() != nullptr) {
		if (!Game::getGame().getDeck(3)->isEmpty())
			draws->getDeck3()->updateAppearance(
			    "../src/assets/rest_detoured/Pioche_niveau_3.png");
		else {
			draws->getDeck3()->setIcon(QIcon());
			draws->getDeck3()->setIconSize(this->size());
			draws->getDeck3()->setDisabled(true);
		}
	}

	// Royal cards and player privileges

	// Top
	int top_privileges = Game::getGame().getOpponent().getPrivilegeNumber();
	topPrivileges->setText(
	    "Privileges: " +
	    QString::fromStdString(std::to_string(top_privileges)));
	switch (Game::getGame().getOpponent().getRoyalCardNumber()) {
	case 0: {
		topRoyal1->setPixmap(QPixmap());
		topRoyal2->setPixmap(QPixmap());
		break;
	}
	case 1: {
		topRoyal1->setPixmap(
		    QPixmap(QString::fromStdString(Game::getGame()
		                                       .getOpponent()
		                                       .getRoyalCards()[0]
		                                       ->getVisual()))
		        .scaled(topRoyal1->size(), Qt::KeepAspectRatio,
		                Qt::SmoothTransformation));
		topRoyal2->setPixmap(QPixmap());
		break;
	}
	case 2: {
		topRoyal1->setPixmap(
		    QPixmap(QString::fromStdString(Game::getGame()
		                                       .getOpponent()
		                                       .getRoyalCards()[0]
		                                       ->getVisual()))
		        .scaled(topRoyal1->size(), Qt::KeepAspectRatio,
		                Qt::SmoothTransformation));
		topRoyal2->setPixmap(
		    QPixmap(QString::fromStdString(Game::getGame()
		                                       .getOpponent()
		                                       .getRoyalCards()[1]
		                                       ->getVisual()))
		        .scaled(topRoyal2->size(), Qt::KeepAspectRatio,
		                Qt::SmoothTransformation));
		break;
	}
	}

	// Bottom
	int bottom_privileges =
	    Game::getGame().getCurrentPlayer().getPrivilegeNumber();
	bottomPrivileges->setText(
	    "Privileges: " +
	    QString::fromStdString(std::to_string(bottom_privileges)));
	switch (Game::getGame().getCurrentPlayer().getRoyalCardNumber()) {
	case 0: {
		bottomRoyal1->setPixmap(QPixmap());
		bottomRoyal2->setPixmap(QPixmap());
		break;
	}
	case 1: {
		bottomRoyal1->setPixmap(
		    QPixmap(QString::fromStdString(Game::getGame()
		                                       .getCurrentPlayer()
		                                       .getRoyalCards()[0]
		                                       ->getVisual()))
		        .scaled(bottomRoyal1->size(), Qt::KeepAspectRatio,
		                Qt::SmoothTransformation));
		bottomRoyal2->setPixmap(QPixmap());

		break;
	}
	case 2: {
		bottomRoyal1->setPixmap(
		    QPixmap(QString::fromStdString(Game::getGame()
		                                       .getCurrentPlayer()
		                                       .getRoyalCards()[0]
		                                       ->getVisual()))
		        .scaled(bottomRoyal1->size(), Qt::KeepAspectRatio,
		                Qt::SmoothTransformation));
		bottomRoyal2->setPixmap(
		    QPixmap(QString::fromStdString(Game::getGame()
		                                       .getCurrentPlayer()
		                                       .getRoyalCards()[1]
		                                       ->getVisual()))
		        .scaled(bottomRoyal2->size(), Qt::KeepAspectRatio,
		                Qt::SmoothTransformation));
		break;
	}
	}
}

void MainWindow::updateBoard() {

	Board::getBoard().printArray();

	// Update token pointers
	for (int i = 0; i < NTOKENS; i++) {
		board->getTokens()[i]->setToken(
		    Board::getBoard().getBoardCaseByIndex(i));
	}

	// Update visuals
	for (int i = 0; i < NTOKENS; i++) {
		board->getTokens()[i]->updateAppearance();
	}
}

void MainWindow::updatePrivileges() {
	int privilege_number = Game::getGame().getPrivilegeNumber();
	QIcon icon(QPixmap(
	    QString::fromStdString("../src/assets/rest_detoured/Privilege.png")));
	switch (privilege_number) {
	case 0:
		board->getPrivilege1()->setIcon(QIcon());
		board->getPrivilege1()->setIconSize(board->getPrivilege1()->size());
		board->getPrivilege2()->setIcon(QIcon());
		board->getPrivilege2()->setIconSize(board->getPrivilege2()->size());
		board->getPrivilege3()->setIcon(QIcon());
		board->getPrivilege3()->setIconSize(board->getPrivilege3()->size());
		break;
	case 1:
		// qDebug() << token->getVisual();
		board->getPrivilege1()->setIcon(icon);
		board->getPrivilege1()->setIconSize(board->getPrivilege1()->size());
		board->getPrivilege2()->setIcon(QIcon());
		board->getPrivilege2()->setIconSize(board->getPrivilege2()->size());
		board->getPrivilege3()->setIcon(QIcon());
		board->getPrivilege3()->setIconSize(board->getPrivilege3()->size());
		break;
	case 2:
		// qDebug() << token->getVisual();
		board->getPrivilege1()->setIcon(icon);
		board->getPrivilege1()->setIconSize(board->getPrivilege1()->size());
		// qDebug() << token->getVisual();
		board->getPrivilege2()->setIcon(icon);
		board->getPrivilege2()->setIconSize(board->getPrivilege2()->size());
		// qDebug() << token->getVisual();
		board->getPrivilege3()->setIcon(QIcon());
		board->getPrivilege3()->setIconSize(board->getPrivilege3()->size());
		break;
	case 3:
		// qDebug() << token->getVisual();
		board->getPrivilege1()->setIcon(icon);
		board->getPrivilege1()->setIconSize(board->getPrivilege1()->size());
		// qDebug() << token->getVisual();
		board->getPrivilege2()->setIcon(icon);
		board->getPrivilege2()->setIconSize(board->getPrivilege2()->size());
		// qDebug() << token->getVisual();
		board->getPrivilege3()->setIcon(icon);
		board->getPrivilege3()->setIconSize(board->getPrivilege3()->size());
		break;
	}
	board->getPrivilege1()->update();
	board->getPrivilege2()->update();
	board->getPrivilege3()->update();
}

void MainWindow::nextAction(int *tmp, Player *j) {

	// qDebug() << "IN NEXT ACTION";
	int nb_choice = j->getOptionalChoices();

	ChoiceDialog dialog(nb_choice, this);
	if (dialog.exec() == QDialog::Accepted) {
		*tmp = dialog.getUserChoice();
	}
}

void MainWindow::YesNo(char *choice, const std::string &string) {

	popupYesNo dialog(this, string);
	if (dialog.exec() == QDialog::Accepted) {
		*choice = dialog.getUserChoice();
	}
}

void MainWindow::showInfo(const string &string) {

	InfoDialog dialog(QString::fromStdString(string), this);
	dialog.exec();
}

void MainWindow::tokenClicked(Qt_token *j) {

	if (j != nullptr && j->getToken() != nullptr) {
		qDebug() << "Token clicked: " << j->getToken()->getVisual();
		setTokenClickIndex(j->getIndex());
	} else {
		setTokenClickIndex(-1);
	}
	j->setDisabled(true);

	MainWindow::getMainWindow().tokenActionDone();
}

void MainWindow::cardClicked(Qt_card *c) {
	if (c != nullptr)
		qDebug() << "Card clicked: " << c->getIndex() << c->getReserved();
	else
		qDebug() << "nullptr card";
	if (c != nullptr and (c->getCard() != nullptr || c->getIndex() < 0))
		setLastCardClick(c);
	else
		qDebug() << "card click problem";
	MainWindow::getMainWindow().cardActionDone();
}

void MainWindow::deactivateButtons() {
	// Tokens
	int nbmax = Token::getMaxTokenNumber();
	for (int i = 0; i < nbmax; i++) {
		board->getBoard().getTokens()[i]->setEnabled(false);
	}

	// Draw 1
	for (int i = 0; i < 5; i++) {
		getDraws()->getTier1()[i]->setEnabled(false);
	}

	// Draw 2
	for (int i = 0; i < 4; i++) {
		getDraws()->getTier2()[i]->setEnabled(false);
	}

	// Draw 3
	for (int i = 0; i < 3; i++) {
		getDraws()->getTier3()[i]->setEnabled(false);
	}

	// Royal cards draw
	for (int i = 0; i < 4; i++) {
		getDraws()->getRoyalCards()[i]->setEnabled(false);
	}

	// The 3 decks

	getDraws()->getDeck1()->setEnabled(false);
	getDraws()->getDeck2()->setEnabled(false);
	getDraws()->getDeck3()->setEnabled(false);

	// Deactivate reserved cards of both players
}

void MainWindow::activateTokens() {
	// Tokens
	int nbmax = Token::getMaxTokenNumber();
	for (int i = 0; i < nbmax; i++) {
		board->getBoard().getTokens()[i]->setEnabled(true);
	}
}

void MainWindow::activateForReserve() {
	// Draw 1
	for (int i = 0; i < Game::getGame().getFirstDraw()->getDrawCards().size();
	     i++) {
		getDraws()->getTier1()[i]->setEnabled(true);
	}

	// Draw 2
	for (int i = 0; i < Game::getGame().getSecondDraw()->getDrawCards().size();
	     i++) {
		getDraws()->getTier2()[i]->setEnabled(true);
	}

	for (int i = 0; i < Game::getGame().getThirdDraw()->getDrawCards().size();
	     i++) {
		getDraws()->getTier3()[i]->setEnabled(true);
	}

	// The 3 decks

	if (!Game::getGame().getDeck(1)->isEmpty())
		getDraws()->getDeck1()->setEnabled(true);
	if (!Game::getGame().getDeck(2)->isEmpty())
		getDraws()->getDeck2()->setEnabled(true);
	if (!Game::getGame().getDeck(3)->isEmpty())
		getDraws()->getDeck3()->setEnabled(true);
}

void MainWindow::activateForBuy() {
	qDebug() << "ACtiate for buy";
	// Draw 1
	for (int i = 0; i < Game::getGame().getFirstDraw()->getDrawCards().size();
	     i++) {
		getDraws()->getTier1()[i]->setEnabled(true);
	}

	// Draw 2
	for (int i = 0; i < Game::getGame().getSecondDraw()->getDrawCards().size();
	     i++) {
		getDraws()->getTier2()[i]->setEnabled(true);
	}

	// Draw 3
	for (int i = 0; i < Game::getGame().getThirdDraw()->getDrawCards().size();
	     i++) {
		getDraws()->getTier3()[i]->setEnabled(true);
	}

	// Reserved cards of the player in question activated in the popup constructor
}

void MainWindow::activateForRoyalCard() {
	// Only non-null card buttons are activated!!! to verify!!!

	for (int i = 0; i < Game::getGame().getRoyalCards().size(); i++) {
		getDraws()->getRoyalCards()[i]->setEnabled(true);
	}
}

void MainWindow::updateWhoPlays() {
	whoPlays->setText(
	    QString::fromStdString("It's " +
	                           Game::getGame().getCurrentPlayer().getName()) +
	    "'s turn");
}

void MainWindow::colorChoice(Color *c, int *nb) {
	// Create the color choice window and get c and nb
	ColorPopup dialog(this);
	if (dialog.exec() == QDialog::Accepted) {
		*c = dialog.getColor();
		*nb = dialog.getNb();
	}
}

void MainWindow::activateTokenColor(const Color &c) {
	// Tokens
	int nbmax = Token::getMaxTokenNumber();
	for (int i = 0; i < nbmax; i++) {
		if (board->getBoard().getTokens()[i]->getToken() != nullptr &&
		    board->getBoard().getTokens()[i]->getToken()->getColor() == c)
			board->getBoard().getTokens()[i]->setEnabled(true);
	}
}

void MainWindow::colorJoker(colorBonus *b) {
	// Create the color choice window and get c and nb
	popupJoker dialog(this);
	if (dialog.exec() == QDialog::Accepted) {
		*b = dialog.getColor();
	}
}

void MainWindow::showStats() {
	QDialog *dialog = new QDialog(this); // Create a new QDialog instance

	vector<Match *> matches = History::getHistory().getMatches();
	std::string txt = "";
	if (matches.size() != 0) {
		txt += "Match history: ";
		txt += "\n\n";

		for (int i = 0; i < matches.size(); i++) {
			txt += "\n";
			txt += "   Match ";
			txt += std::to_string(i + 1);
			txt += "    ||    Score : ";
			txt += std::to_string(matches[i]->getScoreWinner());
			txt += " - ";
			txt += std::to_string(matches[i]->getScoreOpponent());
			txt += "\n";
			txt += "      Winning player:    ";
			txt += matches[i]->getWinner()->getName();
			txt += "\n";
			txt += "      Losing player:    ";
			txt += matches[i]->getOpponent()->getName();
			txt += "\n";
		}
	} else {
		txt += "No matches in history!";
	}

	QLabel *text = new QLabel(QString::fromStdString(txt));
	text->setWordWrap(true);

	// Create a QScrollArea
	QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(text);

	QVBoxLayout *layout =
	    new QVBoxLayout(); // Use QVBoxLayout or another specific layout class
	layout->addWidget(scrollArea);

	dialog->setLayout(layout); // Set layout to the dialog

	dialog->exec();
}

const bool inside(const std::string s, vector<std::string> vect) {
	for (int i = 0; i < vect.size(); ++i) {
		if (vect[i] == s) {
			return true;
		}
	}
	return false;
}

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6) {
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return out.str();
}

void MainWindow::showStatsPlayers() {

	QDialog *dialog = new QDialog(this); // Create a new QDialog instance

	vector<Match *> matches = History::getHistory().getMatches();
	vector<std::string> names;

	std::string txt = "";
	if (matches.size() != 0) {
		txt += "Number of games played:  ";
		txt += to_string(matches.size());
		txt += "\n";

		txt += "Average points to win: ";
		float winner_points = 0, loser_points = 0;
		for (int i = 0; i < matches.size(); ++i) {
			winner_points += (float)matches[i]->getScoreWinner();
			loser_points += (float)matches[i]->getScoreOpponent();
		}
		winner_points = winner_points / matches.size();
		loser_points = loser_points / matches.size();
		txt += to_string_with_precision(winner_points, 2);
		txt += "\n";
		txt += "Average loser points: ";
		txt += to_string_with_precision(loser_points, 2);
		txt += "\n";

		txt += "\n\n";

		txt += "Player statistics  ";
		txt += "\n\n";
		txt += "Total number of players: ";
		txt += std::to_string(History::getHistory().getNbPlayers());
		txt += "\n";
		for (int i = 0; i < matches.size(); i++) {
			// here for the statistics if we haven't already added the person
			// i.e. if their name is not yet in the array

			if (!inside(matches[i]->getWinner()->getName(), names)) {
				// then display the player's stats
				txt += "\n";
				txt += "  Player : ";
				txt += matches[i]->getWinner()->getName();
				txt += "\n";
				txt += "    Number of matches played: ";
				txt += std::to_string(matches[i]->getWinner()->getPlayed());
				txt += "\n";
				txt += "    Number of matches won: ";
				txt += std::to_string(matches[i]->getWinner()->getWins());
				float avg = (float)(matches[i]->getWinner()->getWins()) /
				            (float)(matches[i]->getWinner()->getPlayed());
				txt += " -    Win percentage: ";
				txt += to_string_with_precision(avg * 100, 2);
				txt += "%";
				txt += "\n";
				names.push_back(matches[i]->getWinner()->getName());
			}

			if (!inside(matches[i]->getOpponent()->getName(), names)) {
				// then display the player's stats
				txt += "\n";
				txt += "  Player : ";
				txt += matches[i]->getOpponent()->getName();
				txt += "\n";
				txt += "    Number of matches played: ";
				txt += std::to_string(matches[i]->getOpponent()->getPlayed());
				txt += "\n";
				txt += "    Number of matches won: ";
				txt += std::to_string(matches[i]->getOpponent()->getWins());
				float avg = (float)(matches[i]->getOpponent()->getWins()) /
				            (float)(matches[i]->getOpponent()->getPlayed());
				txt += " -    Win percentage: ";
				txt += to_string_with_precision(avg * 100, 2);
				txt += "%";
				txt += "\n";
				names.push_back(matches[i]->getOpponent()->getName());
			}
		}
	} else {
		txt += "No player statistics";
	}

	QLabel *text = new QLabel(QString::fromStdString(txt));
	text->setWordWrap(true);

	// Create a QScrollArea
	QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(text);

	QVBoxLayout *layout =
	    new QVBoxLayout(); // Use QVBoxLayout or another specific layout class
	layout->addWidget(scrollArea);

	dialog->setLayout(layout); // Set layout to the dialog

	dialog->exec();
}