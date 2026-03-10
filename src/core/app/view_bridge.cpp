#include "view_bridge.h"
#include "board.h"
#include "draw.h"
#include "game.h"
#include "player.h"
#include <optional>
#include <sstream>

namespace UiBridge {
namespace {

constexpr int kPopupTokenSlots = 16;

PlayerPanelViewState buildPlayerPanelViewState(StrategyPlayer &player) {
	PlayerPanelViewState state;
	state.name = player.getName();
	state.score = player.getNbPoints();
	state.privilegeCount = player.getPrivilegeNumber();

	for (const RoyalCard *card : player.getRoyalCards()) {
		state.royalCardVisuals.push_back(card->getVisual());
	}

	return state;
}

std::vector<const Card *> buildDrawCardsViewState(Draw *draw, int slotCount) {
	std::vector<const Card *> cards(slotCount, nullptr);
	auto &drawCards = draw->getDrawCards();

	for (int i = 0; i < slotCount && i < drawCards.size(); ++i) {
		cards[i] = drawCards[i];
	}

	return cards;
}

CardCollectionViewState
buildCardsViewState(const std::vector<const JewelryCard *> &cards,
                    bool allowSelection, bool reservedCards) {
	CardCollectionViewState state;
	for (int i = 0; i < cards.size(); ++i) {
		state.cards.push_back({i, cards[i],
		                       cards[i] != nullptr ? cards[i]->getVisual() : "",
		                       allowSelection, reservedCards});
	}
	return state;
}

TokenCollectionViewState buildTokensViewState(StrategyPlayer &player,
                                              bool allowSelection,
                                              bool filterGold) {
	TokenCollectionViewState state;
	state.title = "Tokens of " + player.getName();
	state.modal = !allowSelection;

	const auto &tokens = player.getToken();
	for (int i = 0; i < kPopupTokenSlots; ++i) {
		const Token *token = i < tokens.size() ? tokens[i] : nullptr;
		const bool enabled = allowSelection && token != nullptr &&
		                     (!filterGold || token->getColor() != Color::gold);
		state.tokens.push_back({i, token, enabled});
	}

	return state;
}

template <typename T>
std::string toStringWithPrecision(const T value, const int precision = 6) {
	std::ostringstream out;
	out.precision(precision);
	out << std::fixed << value;
	return out.str();
}

bool contains(const std::string &value,
              const std::vector<std::string> &values) {
	for (const std::string &current : values) {
		if (current == value) {
			return true;
		}
	}
	return false;
}

} // namespace

MainWindowViewState buildMainWindowViewState() {
	Game &game = Game::getGame();
	MainWindowViewState state;

	state.topPlayer = buildPlayerPanelViewState(game.getOpponent());
	state.bottomPlayer = buildPlayerPanelViewState(game.getCurrentPlayer());
	state.currentTurnLabel =
	    "It's " + game.getCurrentPlayer().getName() + "'s turn";

	for (int i = 0; i < Token::getMaxTokenNumber(); ++i) {
		state.board.tokens.push_back(Board::getBoard().getBoardCaseByIndex(i));
	}
	state.board.privilegeCount = game.getPrivilegeNumber();

	state.draws.tier1Cards = buildDrawCardsViewState(game.getFirstDraw(), 5);
	state.draws.tier2Cards = buildDrawCardsViewState(game.getSecondDraw(), 4);
	state.draws.tier3Cards = buildDrawCardsViewState(game.getThirdDraw(), 3);

	const auto royalCards = game.getRoyalCards();
	state.draws.royalCards.resize(4, nullptr);
	for (int i = 0; i < state.draws.royalCards.size() && i < royalCards.size();
	     ++i) {
		state.draws.royalCards[i] = royalCards[i];
	}

	state.draws.deck1Available = !game.getDeck(1)->isEmpty();
	state.draws.deck2Available = !game.getDeck(2)->isEmpty();
	state.draws.deck3Available = !game.getDeck(3)->isEmpty();
	state.draws.deck1Visual = "../src/assets/rest_detoured/Pioche_niveau_1.png";
	state.draws.deck2Visual = "../src/assets/rest_detoured/Pioche_niveau_2.png";
	state.draws.deck3Visual = "../src/assets/rest_detoured/Pioche_niveau_3.png";

	return state;
}

InteractionState buildDisabledInteractionState() { return {}; }

InteractionState buildTokenActivationState() {
	InteractionState state;
	for (int i = 0; i < Token::getMaxTokenNumber(); ++i) {
		state.enabledBoardTokens.push_back(i);
	}
	return state;
}

InteractionState buildReserveActivationState() {
	Game &game = Game::getGame();
	InteractionState state;

	for (int i = 0; i < game.getFirstDraw()->getDrawCards().size(); ++i) {
		state.enabledTier1Cards.push_back(i);
	}
	for (int i = 0; i < game.getSecondDraw()->getDrawCards().size(); ++i) {
		state.enabledTier2Cards.push_back(i);
	}
	for (int i = 0; i < game.getThirdDraw()->getDrawCards().size(); ++i) {
		state.enabledTier3Cards.push_back(i);
	}

	state.deck1Enabled = !game.getDeck(1)->isEmpty();
	state.deck2Enabled = !game.getDeck(2)->isEmpty();
	state.deck3Enabled = !game.getDeck(3)->isEmpty();
	return state;
}

InteractionState buildBuyActivationState() {
	Game &game = Game::getGame();
	InteractionState state;

	for (int i = 0; i < game.getFirstDraw()->getDrawCards().size(); ++i) {
		state.enabledTier1Cards.push_back(i);
	}
	for (int i = 0; i < game.getSecondDraw()->getDrawCards().size(); ++i) {
		state.enabledTier2Cards.push_back(i);
	}
	for (int i = 0; i < game.getThirdDraw()->getDrawCards().size(); ++i) {
		state.enabledTier3Cards.push_back(i);
	}

	return state;
}

InteractionState buildRoyalActivationState() {
	InteractionState state;
	for (int i = 0; i < Game::getGame().getRoyalCards().size(); ++i) {
		state.enabledRoyalCards.push_back(i);
	}
	return state;
}

InteractionState buildTokenColorActivationState(const Color &color) {
	InteractionState state;

	for (int i = 0; i < Token::getMaxTokenNumber(); ++i) {
		const Token *token = Board::getBoard().getBoardCaseByIndex(i);
		if (token != nullptr && token->getColor() == color) {
			state.enabledBoardTokens.push_back(i);
		}
	}

	return state;
}

CardCollectionViewState
buildCurrentPlayerReservedCardsViewState(bool allowSelection) {
	return buildCardsViewState(
	    Game::getGame().getCurrentPlayer().getReservedCards(), allowSelection,
	    true);
}

CardCollectionViewState buildCurrentPlayerBoughtCardsViewState() {
	return buildCardsViewState(
	    Game::getGame().getCurrentPlayer().getBoughtCards(), false, false);
}

CardCollectionViewState buildOpponentReservedCardsViewState() {
	return buildCardsViewState(Game::getGame().getOpponent().getReservedCards(),
	                           false, false);
}

CardCollectionViewState buildOpponentBoughtCardsViewState() {
	return buildCardsViewState(Game::getGame().getOpponent().getBoughtCards(),
	                           false, false);
}

TokenCollectionViewState
buildCurrentPlayerTokensViewState(bool allowSelection) {
	TokenCollectionViewState state = buildTokensViewState(
	    Game::getGame().getCurrentPlayer(), allowSelection, false);
	state.modal = false;
	return state;
}

TokenCollectionViewState buildOpponentTokensViewState(bool allowSelection) {
	return buildTokensViewState(Game::getGame().getOpponent(), allowSelection,
	                            true);
}

std::vector<ChoiceOption> buildChoiceOptions(int nbChoice) {
	std::vector<ChoiceOption> options;
	int value = 1;

	if (nbChoice % 2 == 1) {
		options.push_back({value++, "Use a privilege"});
	}
	if (nbChoice >= 2) {
		options.push_back({value++, "Fill the board"});
	}
	options.push_back({value++, "Take tokens"});
	options.push_back({value, "Buy a card"});

	return options;
}

ColorChoiceViewState buildColorChoiceViewState() {
	ColorChoiceViewState state;
	state.colors = {{Color::white, "White"}, {Color::blue, "Blue"},
	                {Color::black, "Black"}, {Color::perl, "Pearl"},
	                {Color::red, "Red"},     {Color::green, "Green"}};

	for (int i = 1;
	     i <= Game::getGame().getCurrentPlayer().TokenAmount(Color::gold);
	     ++i) {
		state.amounts.push_back(i);
	}

	return state;
}

JokerChoiceViewState buildJokerChoiceViewState() {
	JokerChoiceViewState state;
	const std::vector<JokerChoiceOption> options = {
	    {colorBonus::white, "White"},
	    {colorBonus::blue, "Blue"},
	    {colorBonus::red, "Red"},
	    {colorBonus::green, "Green"},
	    {colorBonus::black, "Black"}};

	for (const JokerChoiceOption &option : options) {
		if (Game::getGame().getCurrentPlayer().calculateBonus(option.bonus) >
		    0) {
			state.bonuses.push_back(option);
		}
	}

	return state;
}

std::string buildMatchHistoryText() {
	const std::vector<Match *> matches = History::getHistory().getMatches();
	std::string text;

	if (!matches.empty()) {
		text += "Match history: \n\n";

		for (int i = 0; i < matches.size(); ++i) {
			text += "\n";
			text += "   Match ";
			text += std::to_string(i + 1);
			text += "    ||    Score : ";
			text += std::to_string(matches[i]->getScoreWinner());
			text += " - ";
			text += std::to_string(matches[i]->getScoreOpponent());
			text += "\n";
			text += "      Winning player:    ";
			text += matches[i]->getWinner()->getName();
			text += "\n";
			text += "      Losing player:    ";
			text += matches[i]->getOpponent()->getName();
			text += "\n";
		}
	} else {
		text = "No matches in history!";
	}

	return text;
}

std::string buildPlayerStatisticsText() {
	const std::vector<Match *> matches = History::getHistory().getMatches();
	std::vector<std::string> names;
	std::string text;

	if (matches.empty()) {
		return "No player statistics";
	}

	text += "Number of games played:  ";
	text += std::to_string(matches.size());
	text += "\n";

	float winnerPoints = 0;
	float loserPoints = 0;
	for (Match *match : matches) {
		winnerPoints += static_cast<float>(match->getScoreWinner());
		loserPoints += static_cast<float>(match->getScoreOpponent());
	}

	winnerPoints /= matches.size();
	loserPoints /= matches.size();

	text += "Average points to win: ";
	text += toStringWithPrecision(winnerPoints, 2);
	text += "\n";
	text += "Average loser points: ";
	text += toStringWithPrecision(loserPoints, 2);
	text += "\n\n";
	text += "Player statistics  \n\n";
	text += "Total number of players: ";
	text += std::to_string(History::getHistory().getNbPlayers());
	text += "\n";

	for (Match *match : matches) {
		if (!contains(match->getWinner()->getName(), names)) {
			text += "\n";
			text += "  Player : ";
			text += match->getWinner()->getName();
			text += "\n";
			text += "    Number of matches played: ";
			text += std::to_string(match->getWinner()->getPlayed());
			text += "\n";
			text += "    Number of matches won: ";
			text += std::to_string(match->getWinner()->getWins());
			text += " -    Win percentage: ";
			text += toStringWithPrecision(
			    static_cast<float>(match->getWinner()->getWins()) /
			        static_cast<float>(match->getWinner()->getPlayed()) * 100,
			    2);
			text += "%\n";
			names.push_back(match->getWinner()->getName());
		}

		if (!contains(match->getOpponent()->getName(), names)) {
			text += "\n";
			text += "  Player : ";
			text += match->getOpponent()->getName();
			text += "\n";
			text += "    Number of matches played: ";
			text += std::to_string(match->getOpponent()->getPlayed());
			text += "\n";
			text += "    Number of matches won: ";
			text += std::to_string(match->getOpponent()->getWins());
			text += " -    Win percentage: ";
			text += toStringWithPrecision(
			    static_cast<float>(match->getOpponent()->getWins()) /
			        static_cast<float>(match->getOpponent()->getPlayed()) * 100,
			    2);
			text += "%\n";
			names.push_back(match->getOpponent()->getName());
		}
	}

	return text;
}

void applyPlayerSetup(const PlayerSetupData &setup) {
	std::string name1 = setup.name1;
	std::string name2 = setup.name2;
	std::string type1 = setup.type1;
	std::string type2 = setup.type2;
	Game::getGame().setPlayers(name1, name2, type1, type2);
}

} // namespace UiBridge
