#ifndef LO21_SPLENDOR_DUEL_VIEW_BRIDGE_H
#define LO21_SPLENDOR_DUEL_VIEW_BRIDGE_H

#include "card.h"
#include "history.h"
#include "tokens.h"
#include <string>
#include <vector>

namespace UiBridge {

struct PlayerSetupData {
	std::string name1;
	std::string name2;
	std::string type1;
	std::string type2;
};

struct CardSlotViewState {
	int index = -1;
	const Card *card = nullptr;
	std::string visual;
	bool enabled = false;
	bool reserved = false;
};

struct TokenSlotViewState {
	int index = -1;
	const Token *token = nullptr;
	bool enabled = false;
};

struct CardCollectionViewState {
	std::vector<CardSlotViewState> cards;
	bool modal = true;
};

struct TokenCollectionViewState {
	std::string title;
	std::vector<TokenSlotViewState> tokens;
	bool modal = true;
};

struct PlayerPanelViewState {
	std::string name;
	int score = 0;
	int privilegeCount = 0;
	std::vector<std::string> royalCardVisuals;
};

struct BoardViewState {
	std::vector<const Token *> tokens;
	int privilegeCount = 0;
};

struct DrawsViewState {
	std::vector<const Card *> tier1Cards;
	std::vector<const Card *> tier2Cards;
	std::vector<const Card *> tier3Cards;
	std::vector<const Card *> royalCards;
	bool deck1Available = false;
	bool deck2Available = false;
	bool deck3Available = false;
	std::string deck1Visual;
	std::string deck2Visual;
	std::string deck3Visual;
};

struct MainWindowViewState {
	PlayerPanelViewState topPlayer;
	PlayerPanelViewState bottomPlayer;
	std::string currentTurnLabel;
	BoardViewState board;
	DrawsViewState draws;
};

struct InteractionState {
	std::vector<int> enabledBoardTokens;
	std::vector<int> enabledTier1Cards;
	std::vector<int> enabledTier2Cards;
	std::vector<int> enabledTier3Cards;
	std::vector<int> enabledRoyalCards;
	bool deck1Enabled = false;
	bool deck2Enabled = false;
	bool deck3Enabled = false;
};

struct ChoiceOption {
	int value = -1;
	std::string label;
};

struct ColorChoiceOption {
	Color color = Color::blue;
	std::string label;
};

struct ColorChoiceViewState {
	std::vector<ColorChoiceOption> colors;
	std::vector<int> amounts;
};

struct JokerChoiceOption {
	colorBonus bonus = colorBonus::white;
	std::string label;
};

struct JokerChoiceViewState {
	std::vector<JokerChoiceOption> bonuses;
};

MainWindowViewState buildMainWindowViewState();
InteractionState buildDisabledInteractionState();
InteractionState buildTokenActivationState();
InteractionState buildReserveActivationState();
InteractionState buildBuyActivationState();
InteractionState buildRoyalActivationState();
InteractionState buildTokenColorActivationState(const Color &color);

CardCollectionViewState
buildCurrentPlayerReservedCardsViewState(bool allowSelection);
CardCollectionViewState buildCurrentPlayerBoughtCardsViewState();
CardCollectionViewState buildOpponentReservedCardsViewState();
CardCollectionViewState buildOpponentBoughtCardsViewState();

TokenCollectionViewState buildCurrentPlayerTokensViewState(bool allowSelection);
TokenCollectionViewState buildOpponentTokensViewState(bool allowSelection);

std::vector<ChoiceOption> buildChoiceOptions(int nbChoice);
ColorChoiceViewState buildColorChoiceViewState();
JokerChoiceViewState buildJokerChoiceViewState();

std::string buildMatchHistoryText();
std::string buildPlayerStatisticsText();
void applyPlayerSetup(const PlayerSetupData &setup);

} // namespace UiBridge

#endif // LO21_SPLENDOR_DUEL_VIEW_BRIDGE_H
