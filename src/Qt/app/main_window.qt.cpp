#include "main_window.qt.h"
#include "color_popup.qt.h"
#include "joker_popup.qt.h"
#include "view_bridge.h"
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QVBoxLayout>
#include <array>

namespace {

constexpr auto kBackgroundStyle =
    "background-image: url('../src/assets/background.jpg'); "
    "background-position: center;";
constexpr auto kTransparentStyle = "background: transparent;";
constexpr auto kWhiteButtonStyle = "color: rgba(255, 255, 255, 255);";
constexpr auto kWhiteLabelStyle = "QLabel { color: white; }";
constexpr auto kPlayerBannerStyle =
    "QLabel { color: white; background-color: white; border: 1px solid "
    "white; }";

const QSize kRoyalLabelSize(50, 80);
const QSize kPopupCardSize(75, 105);
const QSize kPopupTokenSize(60, 60);
constexpr int kPopupColumns = 4;
constexpr int kPopupTokenSlots = 16;

template <typename Slot>
QPushButton *createStyledButton(const QString &text, QWidget *parent,
                                const QObject *receiver, Slot slot) {
	auto *button = new QPushButton(text, parent);
	button->setStyleSheet(kWhiteButtonStyle);
	QObject::connect(button, &QPushButton::clicked, receiver, slot);
	return button;
}

QLabel *createRoyalLabel(QWidget *parent) {
	auto *label = new QLabel(parent);
	label->setStyleSheet(kTransparentStyle);
	label->setFixedSize(kRoyalLabelSize);
	label->setPixmap(QPixmap());
	return label;
}

QLabel *createPlayerBanner(const QString &text, QWidget *parent) {
	auto *label = new QLabel(text, parent);
	label->setStyleSheet(kPlayerBannerStyle);
	return label;
}

QDialog *createBackgroundDialog(QWidget *parent) {
	auto *dialog = new QDialog(parent);
	dialog->setStyleSheet(kBackgroundStyle);
	return dialog;
}

void showScrollableTextDialog(QWidget *parent, const std::string &text) {
	auto *dialog = createBackgroundDialog(parent);
	QLabel *label = new QLabel(QString::fromStdString(text), dialog);
	label->setWordWrap(true);

	QScrollArea *scrollArea = new QScrollArea(dialog);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(label);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(scrollArea);
	dialog->setLayout(layout);
	dialog->exec();
}

void clearCardButton(Qt_card *card, const QSize &iconSize) {
	card->setCard(nullptr);
	card->setIcon(QIcon());
	card->setIconSize(iconSize);
}

void updateRoyalLabel(QLabel *label, const std::string &visual) {
	if (visual.empty()) {
		label->setPixmap(QPixmap());
		return;
	}

	label->setPixmap(QPixmap(QString::fromStdString(visual))
	                     .scaled(label->size(), Qt::KeepAspectRatio,
	                             Qt::SmoothTransformation));
}

void updatePlayerPanel(QLabel *nameLabel, QLCDNumber *scoreDisplay,
                       QLabel *privilegesLabel, QLabel *firstRoyal,
                       QLabel *secondRoyal,
                       const UiBridge::PlayerPanelViewState &player) {
	nameLabel->setText(QString::fromStdString(player.name));
	scoreDisplay->display(player.score);
	privilegesLabel->setText(
	    "Privileges: " +
	    QString::fromStdString(std::to_string(player.privilegeCount)));
	updateRoyalLabel(firstRoyal, player.royalCardVisuals.empty()
	                                 ? ""
	                                 : player.royalCardVisuals[0]);
	updateRoyalLabel(secondRoyal, player.royalCardVisuals.size() < 2
	                                  ? ""
	                                  : player.royalCardVisuals[1]);
}

void updatePrivilegeTokens(Qt_Board *board, int privilegeCount) {
	std::array<Qt_token *, 3> privilegeTokens = {
	    board->getPrivilege1(), board->getPrivilege2(), board->getPrivilege3()};
	QIcon icon(QPixmap(
	    QString::fromStdString("../src/assets/rest_detoured/Privilege.png")));

	for (int i = 0; i < privilegeTokens.size(); ++i) {
		privilegeTokens[i]->setIcon(i < privilegeCount ? icon : QIcon());
		privilegeTokens[i]->setIconSize(privilegeTokens[i]->size());
		privilegeTokens[i]->update();
	}
}

void applyCardSlotState(Qt_card *card, const UiBridge::CardSlotViewState &state,
                        const QSize &emptyIconSize) {
	card->setIndice(state.index);
	card->setReserved(state.reserved);
	card->setDisabled(!state.enabled);
	card->setCard(state.card);

	if (state.card != nullptr) {
		card->updateAppearance();
	} else if (!state.visual.empty()) {
		card->updateAppearance(state.visual);
	} else {
		clearCardButton(card, emptyIconSize);
	}
}

void populateCardsGrid(QDialog *dialog,
                       const UiBridge::CardCollectionViewState &state) {
	auto *layout = new QGridLayout(dialog);

	for (int i = 0; i < state.cards.size(); ++i) {
		Qt_card *card = new Qt_card(dialog);
		card->setFixedSize(kPopupCardSize);
		applyCardSlotState(card, state.cards[i], kPopupCardSize);
		QObject::connect(card, &Qt_card::cardClicked,
		                 &MainWindow::getMainWindow(),
		                 &MainWindow::cardClicked);
		if (state.cards[i].enabled) {
			QObject::connect(card, &Qt_card::cardClicked, dialog,
			                 &QDialog::accept);
		}
		layout->addWidget(card, i / kPopupColumns, i % kPopupColumns);
	}

	dialog->setLayout(layout);
}

void populateTokensGrid(MainWindow *window, QDialog *dialog,
                        const UiBridge::TokenCollectionViewState &state) {
	auto *verticalLayout = new QVBoxLayout(dialog);

	auto *name = new QLabel(QString::fromStdString(state.title), dialog);
	name->setStyleSheet(kWhiteLabelStyle);
	verticalLayout->addWidget(name, Qt::AlignCenter);

	auto *layout = new QGridLayout(dialog);
	auto *gridWidget = new QWidget(dialog);
	gridWidget->setLayout(layout);

	for (int i = 0; i < state.tokens.size(); ++i) {
		Qt_token *token = new Qt_token(dialog);
		token->setIndice(state.tokens[i].index);
		token->setToken(state.tokens[i].token);
		token->setFixedSize(kPopupTokenSize);
		token->setStyleSheet("border: 1px solid black;");
		layout->addWidget(token, i / kPopupColumns, i % kPopupColumns);

		token->setDisabled(!state.tokens[i].enabled);
		if (state.tokens[i].enabled) {
			QObject::connect(token, &Qt_token::tokenClicked,
			                 &MainWindow::getMainWindow(),
			                 &MainWindow::tokenClicked);
			if (state.modal) {
				QObject::connect(token, &Qt_token::tokenClicked, dialog,
				                 &QDialog::accept);
			}
		}
		token->updateAppearance();
	}

	for (int i = 0; i < kPopupColumns; ++i) {
		layout->setRowStretch(i, 0);
		layout->setColumnStretch(i, 0);
	}

	verticalLayout->addWidget(gridWidget);
	dialog->setLayout(verticalLayout);
	window->setCurrentDialog(dialog);
}

} // namespace

MainWindow::Handler MainWindow::handler;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), buyingCard(false), stealingToken(false),
      token_click_index(-1), last_card_click(nullptr), discarding(false),
      current_dialog(nullptr) {
	topRoyal1 = createRoyalLabel(this);
	topRoyal2 = createRoyalLabel(this);
	bottomRoyal1 = createRoyalLabel(this);
	bottomRoyal2 = createRoyalLabel(this);

	topPrivileges = new QLabel(this);
	topPrivileges->setStyleSheet(kWhiteLabelStyle);
	bottomPrivileges = new QLabel(this);
	bottomPrivileges->setStyleSheet(kWhiteLabelStyle);

	topPlayerNameLabel = createPlayerBanner("Player 1", this);
	bottomPlayerNameLabel = createPlayerBanner("Player 2", this);

	QWidget *centralWidget = new QWidget(this);

	centralWidget->setStyleSheet(kBackgroundStyle);

	QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
	mainLayout->setSpacing(16);

	whoPlays = createPlayerBanner("Current player", this);
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
	viewCardsButtonTop = createStyledButton("View bought cards", this, this,
	                                        &MainWindow::showBoughtCardsTop);
	viewTokensButtonTop = createStyledButton("View tokens", this, this,
	                                         &MainWindow::showTokensTop);
	viewReservedCardsButtonTop = createStyledButton(
	    "View reserved cards", this, this, &MainWindow::showReservedCardsTop);

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
	    boardLayout);           // Add the board and button to the middle layout
	middleLayout->addStretch(); // Stretch to put the draws to the right
	draws->setStyleSheet(kBackgroundStyle);
	middleLayout->addWidget(draws);
	mainLayout->addWidget(middleContainer);

	// ######## VICTORY CONDITIONS AND RULES ########

	QVBoxLayout *rules = new QVBoxLayout;

	// Add rules button
	QPushButton *viewRules =
	    createStyledButton("View rules", this, this, &MainWindow::openWebLink);
	QPushButton *viewStats =
	    createStyledButton("History", this, this, &MainWindow::showStats);
	QPushButton *viewStatsPlayers = createStyledButton(
	    "Statistics", this, this, &MainWindow::showStatsPlayers);
	viewRules->setFixedWidth(397 / 3);
	viewStats->setFixedWidth(397 / 3);
	viewStatsPlayers->setFixedWidth(397 / 3);

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
	viewCardsButtonBottom = createStyledButton(
	    "View bought cards", this, this, &MainWindow::showBoughtCardsBottom);
	viewTokensButtonBottom = createStyledButton("View tokens", this, this,
	                                            &MainWindow::showTokensBottom);
	viewReservedCardsButtonBottom =
	    createStyledButton("View reserved cards", this, this,
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

bool MainWindow::askNames(UiBridge::PlayerSetupData &setup) {
	InputPopup popup(this);
	popup.setModal(true);
	if (popup.exec() != QDialog::Accepted) {
		return false;
	}

	setup = popup.getPlayerSetupData();
	return true;
}

void MainWindow::showTokensBottom() {
	QDialog *tokensDialog = createBackgroundDialog(this);
	populateTokensGrid(
	    this, tokensDialog,
	    UiBridge::buildCurrentPlayerTokensViewState(getDiscarding()));
	tokensDialog->show();
}

void MainWindow::showReservedCardsBottom() {
	QDialog *cardsDialog = createBackgroundDialog(this);
	populateCardsGrid(
	    cardsDialog,
	    UiBridge::buildCurrentPlayerReservedCardsViewState(getBuyingCard()));
	cardsDialog->exec(); // Show it
}

void MainWindow::showBoughtCardsBottom() {
	QDialog *cardsDialog = createBackgroundDialog(this);
	populateCardsGrid(cardsDialog,
	                  UiBridge::buildCurrentPlayerBoughtCardsViewState());
	cardsDialog->exec(); // Show it
}

void MainWindow::showBoughtCardsTop() {
	QDialog *cardsDialog = createBackgroundDialog(this);
	populateCardsGrid(cardsDialog,
	                  UiBridge::buildOpponentBoughtCardsViewState());
	cardsDialog->exec(); // Show it
}

void MainWindow::showTokensTop() {
	QDialog *tokensDialog = createBackgroundDialog(this);
	populateTokensGrid(
	    this, tokensDialog,
	    UiBridge::buildOpponentTokensViewState(getStealingToken()));
	tokensDialog->exec();
}

void MainWindow::showReservedCardsTop() {
	QDialog *cardsDialog = createBackgroundDialog(this);
	populateCardsGrid(cardsDialog,
	                  UiBridge::buildOpponentReservedCardsViewState());
	cardsDialog->exec(); // Show it
}

void MainWindow::fillBoard() {
	// Put the action here
}

void MainWindow::openWebLink() {
	QDesktopServices::openUrl(
	    QUrl("https://cdn.1j1ju.com/medias/da/39/6a-splendor-duel-regle.pdf"));
}

void MainWindow::render(const UiBridge::MainWindowViewState &state) {
	updatePlayerPanel(topPlayerNameLabel, topScoreDisplay, topPrivileges,
	                  topRoyal1, topRoyal2, state.topPlayer);
	updatePlayerPanel(bottomPlayerNameLabel, bottomScoreDisplay,
	                  bottomPrivileges, bottomRoyal1, bottomRoyal2,
	                  state.bottomPlayer);
	whoPlays->setText(QString::fromStdString(state.currentTurnLabel));

	for (int i = 0;
	     i < board->getTokens().size() && i < state.board.tokens.size(); ++i) {
		board->getTokens()[i]->setToken(state.board.tokens[i]);
		board->getTokens()[i]->updateAppearance();
	}
	updatePrivilegeTokens(board, state.board.privilegeCount);

	const auto applyDrawState = [&](std::vector<Qt_card *> &widgets,
	                                const std::vector<const Card *> &cards) {
		for (int i = 0; i < widgets.size(); ++i) {
			widgets[i]->setCard(i < cards.size() ? cards[i] : nullptr);
			if (i < cards.size() && cards[i] != nullptr) {
				widgets[i]->updateAppearance();
			} else {
				clearCardButton(widgets[i], widgets[i]->size());
				widgets[i]->setDisabled(true);
			}
		}
	};

	applyDrawState(draws->getTier1(), state.draws.tier1Cards);
	applyDrawState(draws->getTier2(), state.draws.tier2Cards);
	applyDrawState(draws->getTier3(), state.draws.tier3Cards);
	applyDrawState(draws->getRoyalCards(), state.draws.royalCards);

	const UiBridge::CardSlotViewState deck1State = {
	    -1, nullptr,
	    state.draws.deck1Available ? state.draws.deck1Visual : std::string(),
	    false, false};
	const UiBridge::CardSlotViewState deck2State = {
	    -2, nullptr,
	    state.draws.deck2Available ? state.draws.deck2Visual : std::string(),
	    false, false};
	const UiBridge::CardSlotViewState deck3State = {
	    -3, nullptr,
	    state.draws.deck3Available ? state.draws.deck3Visual : std::string(),
	    false, false};

	applyCardSlotState(draws->getDeck1(), deck1State,
	                   draws->getDeck1()->size());
	applyCardSlotState(draws->getDeck2(), deck2State,
	                   draws->getDeck2()->size());
	applyCardSlotState(draws->getDeck3(), deck3State,
	                   draws->getDeck3()->size());
}

void MainWindow::applyInteractionState(
    const UiBridge::InteractionState &state) {
	const auto setIndexesEnabled = [](auto &widgets,
	                                  const std::vector<int> &indices) {
		for (auto *widget : widgets) {
			widget->setEnabled(false);
		}
		for (int index : indices) {
			if (index >= 0 && index < widgets.size()) {
				widgets[index]->setEnabled(true);
			}
		}
	};

	setIndexesEnabled(board->getTokens(), state.enabledBoardTokens);
	setIndexesEnabled(draws->getTier1(), state.enabledTier1Cards);
	setIndexesEnabled(draws->getTier2(), state.enabledTier2Cards);
	setIndexesEnabled(draws->getTier3(), state.enabledTier3Cards);
	setIndexesEnabled(draws->getRoyalCards(), state.enabledRoyalCards);

	draws->getDeck1()->setEnabled(state.deck1Enabled);
	draws->getDeck2()->setEnabled(state.deck2Enabled);
	draws->getDeck3()->setEnabled(state.deck3Enabled);
}

void MainWindow::nextAction(int *tmp, int nbChoice) {
	std::vector<std::pair<int, QString>> options;
	for (const auto &option : UiBridge::buildChoiceOptions(nbChoice)) {
		options.push_back({option.value, QString::fromStdString(option.label)});
	}

	ChoiceDialog dialog(options, this);
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

void MainWindow::colorChoice(Color *c, int *nb) {
	ColorPopup dialog(UiBridge::buildColorChoiceViewState(), this);
	if (dialog.exec() == QDialog::Accepted) {
		*c = dialog.getColor();
		*nb = dialog.getNb();
	}
}

void MainWindow::colorJoker(colorBonus *b) {
	popupJoker dialog(UiBridge::buildJokerChoiceViewState(), this);
	if (dialog.exec() == QDialog::Accepted) {
		*b = dialog.getColor();
	}
}

void MainWindow::showStats() {
	showScrollableTextDialog(this, UiBridge::buildMatchHistoryText());
}

void MainWindow::showStatsPlayers() {
	showScrollableTextDialog(this, UiBridge::buildPlayerStatisticsText());
}
