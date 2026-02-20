#include <gtest/gtest.h>
#include "../src/core/headers/game.h"
#include "../src/core/headers/player.h"
#include "../src/core/headers/card.h"
#include "../src/core/headers/board.h"
#include "../src/core/headers/bag.h"
#include "../src/core/headers/privilege.h"

class GameInitializationTest : public ::testing::Test {
protected:
    static Game* game;

    static void SetUpTestSuite() {
        game = &Game::getGame();
        std::string n1 = "Player1", n2 = "Player2", c1 = "Player", c2 = "Player";
        game->setPlayers(n1, n2, c1, c2);
    }

    static void TearDownTestSuite() {
        Game::free();
    }
};

Game* GameInitializationTest::game = nullptr;

TEST_F(GameInitializationTest, GameStateAndFlags) {
    ASSERT_NE(game, nullptr);
    EXPECT_FALSE(game->isFinished());
    EXPECT_EQ(game->getRoundCount(), 0);
}

TEST_F(GameInitializationTest, DrawsAndDecks) {
    // Verify draws are created with correct sizes
    Draw* draw1 = game->getFirstDraw();
    Draw* draw2 = game->getSecondDraw();
    Draw* draw3 = game->getThirdDraw();

    ASSERT_NE(draw1, nullptr);
    ASSERT_NE(draw2, nullptr);
    ASSERT_NE(draw3, nullptr);

    EXPECT_EQ(draw1->getCardsNumber(), 5);
    EXPECT_EQ(draw2->getCardsNumber(), 4);
    EXPECT_EQ(draw3->getCardsNumber(), 3);

    // Verify 3 decks exist
    EXPECT_NO_THROW(game->getDeck(1));
    EXPECT_NO_THROW(game->getDeck(2));
    EXPECT_NO_THROW(game->getDeck(3));
}

TEST_F(GameInitializationTest, CardsAndPrivileges) {
    EXPECT_EQ(game->getRoyalCards().size(), 4);

    vector<const JewelryCard*> jewelry_cards = game->getJewelryCards();
    EXPECT_GT(jewelry_cards.size(), 0);
    EXPECT_LE(jewelry_cards.size(), JewelryCard::getMaxJewelryCard());

    EXPECT_EQ(game->getPrivilegeNumber(), 2); // 1 privilege given to opponent during setPlayers()
}

TEST_F(GameInitializationTest, BagIsEmpty) {
    Bag& bag = Bag::get();
    EXPECT_EQ(bag.getTokenNumber(), 0);
}

TEST_F(GameInitializationTest, BoardHasAllTokens) {
    Board& board = Board::getBoard();
    int token_count = 0;
    for (int i = 0; i < 25; i++) {
        if (board.getBoardCaseByIndex(i) != nullptr) {
            token_count++;
        }
    }
    EXPECT_EQ(token_count, 25);
}

TEST_F(GameInitializationTest, TokenColorDistribution) {
    Board& board = Board::getBoard();
    int blue = 0, red = 0, green = 0, white = 0, black = 0, pearl = 0, gold = 0;
    for (int i = 0; i < 25; i++) {
        const Token* token = board.getBoardCaseByIndex(i);
        if (token != nullptr) {
            switch (token->getColor()) {
                case Color::blue: blue++; break;
                case Color::red: red++; break;
                case Color::green: green++; break;
                case Color::white: white++; break;
                case Color::black: black++; break;
                case Color::perl: pearl++; break;
                case Color::gold: gold++; break;
            }
        }
    }
    EXPECT_EQ(blue, 4);
    EXPECT_EQ(red, 4);
    EXPECT_EQ(green, 4);
    EXPECT_EQ(white, 4);
    EXPECT_EQ(black, 4);
    EXPECT_EQ(pearl, 2);
    EXPECT_EQ(gold, 3);
}

TEST_F(GameInitializationTest, PlayerInitialState) {
    StrategyPlayer& current_player = game->getCurrentPlayer();
    StrategyPlayer& opponent = game->getOpponent();

    // Verify initial player state (0 tokens, 0 cards, 0 privileges, 0 points/crowns)
    EXPECT_EQ(current_player.getTokenNumber(), 0);
    EXPECT_EQ(opponent.getTokenNumber(), 0);
    EXPECT_EQ(current_player.getNbPoints(), 0);
    EXPECT_EQ(opponent.getNbPoints(), 0);
    EXPECT_EQ(current_player.getCrownNumber(), 0);
    EXPECT_EQ(opponent.getCrownNumber(), 0);
}
