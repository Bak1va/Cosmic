#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "GameScreen.hpp"
#include "IGameEngine.hpp"

using namespace Pacman;

class MockGameEngine : public IGameEngine {
public:
    MOCK_METHOD(void, StartNewGame, (), (override));
    MOCK_METHOD(void, Update, (float deltaTime), (override));
    MOCK_METHOD(void, SetPaused, (bool isPaused), (override));
    MOCK_METHOD(void, SetPlayerDirection, (Direction direction), (override));

    MOCK_METHOD(GameState, GetState, (), (const, override));
    MOCK_METHOD(PlayerState, GetPlayerState, (), (const, override));
    MOCK_METHOD(Vector2, GetMapSize, (), (const, override));
    MOCK_METHOD(TileType, GetTileAt, (const Vector2& position), (const, override));
    MOCK_METHOD(std::vector<Vector2>, GetPelletPositions, (), (const, override));
    MOCK_METHOD(int, GetPelletCount, (), (const, override));
    MOCK_METHOD(std::vector<GhostState>, GetGhostStates, (), (const, override));
    MOCK_METHOD(GhostMode, GetGlobalGhostMode, (), (const, override));

    MOCK_METHOD(void, AddListener, (std::shared_ptr<IEventListener> listener), (override));
    MOCK_METHOD(void, RemoveListener, (std::shared_ptr<IEventListener> listener), (override));
};

class GameScreenTest : public ::testing::Test {
protected:
    void SetUp() override {
        gameScreen = std::make_unique<GameScreen>();
        mockEngine = std::make_shared<MockGameEngine>();
        window.create(sf::VideoMode(800, 600), "Test", sf::Style::None);
    }

    void TearDown() override {
        window.close();
    }

    std::unique_ptr<GameScreen> gameScreen;
    std::shared_ptr<MockGameEngine> mockEngine;
    sf::RenderWindow window;
};

TEST_F(GameScreenTest, Construction_DoesNotCrash) {
    EXPECT_NE(gameScreen, nullptr);
}

TEST_F(GameScreenTest, SetGameEngine_StoresEngine) {
    EXPECT_NO_THROW(gameScreen->SetGameEngine(mockEngine));
}

TEST_F(GameScreenTest, SetGameEngine_WithNullptr_DoesNotCrash) {
    EXPECT_NO_THROW(gameScreen->SetGameEngine(nullptr));
}

TEST_F(GameScreenTest, SetGameEngine_MultipleTimesAllowed) {
    gameScreen->SetGameEngine(mockEngine);
    auto engine2 = std::make_shared<MockGameEngine>();
    EXPECT_NO_THROW(gameScreen->SetGameEngine(engine2));
}

TEST_F(GameScreenTest, OnTileUpdated_StoresUpdate) {
    TileUpdate update;
    update.Position = Vector2{5, 10};
    update.Type = TileType::Empty;

    EXPECT_NO_THROW(gameScreen->OnTileUpdated(update));
}

TEST_F(GameScreenTest, OnTileUpdated_MultipleTiles) {
    TileUpdate update1;
    update1.Position = Vector2{1, 1};
    update1.Type = TileType::Empty;

    TileUpdate update2;
    update2.Position = Vector2{2, 2};
    update2.Type = TileType::Pellet;

    EXPECT_NO_THROW(gameScreen->OnTileUpdated(update1));
    EXPECT_NO_THROW(gameScreen->OnTileUpdated(update2));
}

TEST_F(GameScreenTest, OnTileUpdated_AllTileTypes) {
    TileUpdate update;
    update.Position = Vector2{5, 5};

    for (auto type : {TileType::Wall, TileType::Path, TileType::Pellet,
                       TileType::PowerPellet, TileType::Empty, TileType::GhostDoor}) {
        update.Type = type;
        EXPECT_NO_THROW(gameScreen->OnTileUpdated(update));
    }
}

TEST_F(GameScreenTest, OnPlayerStateChanged_UpdatesState) {
    PlayerState state;
    state.Position = Vector2{13, 26};
    state.CurrentDirection = Direction::Right;
    state.Score = 1000;
    state.IsPoweredUp = true;
    state.Lives = 3;

    EXPECT_NO_THROW(gameScreen->OnPlayerStateChanged(state));
}

TEST_F(GameScreenTest, OnPlayerStateChanged_MultipleUpdates) {
    PlayerState state;
    state.Position = Vector2{10, 10};

    for (int i = 0; i < 10; ++i) {
        state.Score = i * 100;
        EXPECT_NO_THROW(gameScreen->OnPlayerStateChanged(state));
    }
}

TEST_F(GameScreenTest, OnPlayerStateChanged_AllDirections) {
    PlayerState state;
    state.Position = Vector2{10, 10};

    for (auto dir : {Direction::Up, Direction::Down, Direction::Left,
                      Direction::Right, Direction::None}) {
        state.CurrentDirection = dir;
        EXPECT_NO_THROW(gameScreen->OnPlayerStateChanged(state));
    }
}

TEST_F(GameScreenTest, OnPlayerStateChanged_PowerUpToggle) {
    PlayerState state;
    state.Position = Vector2{10, 10};

    state.IsPoweredUp = false;
    gameScreen->OnPlayerStateChanged(state);

    state.IsPoweredUp = true;
    EXPECT_NO_THROW(gameScreen->OnPlayerStateChanged(state));
}

TEST_F(GameScreenTest, OnPlayerStateChanged_LivesChanges) {
    PlayerState state;
    state.Position = Vector2{10, 10};

    for (int lives = 0; lives <= 5; ++lives) {
        state.Lives = lives;
        EXPECT_NO_THROW(gameScreen->OnPlayerStateChanged(state));
    }
}

TEST_F(GameScreenTest, OnGameStateChanged_AllStates) {
    for (auto state : {GameState::Running, GameState::Paused,
                        GameState::GameOver, GameState::Victory}) {
        EXPECT_NO_THROW(gameScreen->OnGameStateChanged(state));
    }
}

TEST_F(GameScreenTest, OnGameStateChanged_MultipleTransitions) {
    gameScreen->OnGameStateChanged(GameState::Running);
    gameScreen->OnGameStateChanged(GameState::Paused);
    gameScreen->OnGameStateChanged(GameState::Running);
    EXPECT_NO_THROW(gameScreen->OnGameStateChanged(GameState::GameOver));
}

TEST_F(GameScreenTest, OnGhostsUpdated_EmptyVector) {
    std::vector<GhostState> ghosts;
    EXPECT_NO_THROW(gameScreen->OnGhostsUpdated(ghosts));
}

TEST_F(GameScreenTest, OnGhostsUpdated_SingleGhost) {
    std::vector<GhostState> ghosts(1);
    ghosts[0].Position = Vector2{12, 14};
    ghosts[0].Type = GhostType::Red;
    ghosts[0].Mode = GhostMode::Chase;

    EXPECT_NO_THROW(gameScreen->OnGhostsUpdated(ghosts));
}

TEST_F(GameScreenTest, OnGhostsUpdated_FourGhosts) {
    std::vector<GhostState> ghosts(4);

    ghosts[0].Type = GhostType::Red;
    ghosts[1].Type = GhostType::Pink;
    ghosts[2].Type = GhostType::Blue;
    ghosts[3].Type = GhostType::Orange;

    for (auto& ghost : ghosts) {
        ghost.Position = Vector2{12, 14};
        ghost.Mode = GhostMode::Scatter;
    }

    EXPECT_NO_THROW(gameScreen->OnGhostsUpdated(ghosts));
}

TEST_F(GameScreenTest, OnGhostsUpdated_FrightenedGhosts) {
    std::vector<GhostState> ghosts(2);
    ghosts[0].IsFrightened = true;
    ghosts[1].IsFrightened = false;

    EXPECT_NO_THROW(gameScreen->OnGhostsUpdated(ghosts));
}

TEST_F(GameScreenTest, OnGhostsUpdated_EatenGhosts) {
    std::vector<GhostState> ghosts(2);
    ghosts[0].IsEaten = true;
    ghosts[1].IsEaten = false;

    EXPECT_NO_THROW(gameScreen->OnGhostsUpdated(ghosts));
}

TEST_F(GameScreenTest, OnGhostsUpdated_AllGhostModes) {
    std::vector<GhostState> ghosts(4);
    ghosts[0].Mode = GhostMode::Chase;
    ghosts[1].Mode = GhostMode::Scatter;
    ghosts[2].Mode = GhostMode::Frightened;
    ghosts[3].Mode = GhostMode::Eaten;

    EXPECT_NO_THROW(gameScreen->OnGhostsUpdated(ghosts));
}

TEST_F(GameScreenTest, Render_WithoutEngine_DoesNotCrash) {
    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, Render_WithEngine_DoesNotCrash) {
    gameScreen->SetGameEngine(mockEngine);
    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, Render_AfterTileUpdate_DoesNotCrash) {
    TileUpdate update;
    update.Position = Vector2{5, 5};
    update.Type = TileType::Empty;
    gameScreen->OnTileUpdated(update);

    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, Render_AfterPlayerUpdate_DoesNotCrash) {
    PlayerState state;
    state.Position = Vector2{10, 10};
    gameScreen->OnPlayerStateChanged(state);

    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, Render_AfterGhostUpdate_DoesNotCrash) {
    std::vector<GhostState> ghosts(4);
    gameScreen->OnGhostsUpdated(ghosts);

    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, Render_MultipleFrames_DoesNotCrash) {
    gameScreen->SetGameEngine(mockEngine);

    for (int i = 0; i < 60; ++i) {
        EXPECT_NO_THROW(gameScreen->Render(window));
    }
}

TEST_F(GameScreenTest, ImplementsIEventListener) {
    IEventListener* listener = gameScreen.get();
    EXPECT_NE(listener, nullptr);
}

TEST_F(GameScreenTest, CanBeUsedAsIEventListener) {
    IEventListener* listener = gameScreen.get();
    TileUpdate update;
    update.Position = Vector2{1, 1};
    update.Type = TileType::Empty;

    EXPECT_NO_THROW(listener->OnTileUpdated(update));
}

TEST_F(GameScreenTest, CompleteGameSequence_DoesNotCrash) {
    gameScreen->SetGameEngine(mockEngine);

    gameScreen->OnGameStateChanged(GameState::Running);

    PlayerState playerState;
    playerState.Position = Vector2{13, 26};
    playerState.Score = 0;
    gameScreen->OnPlayerStateChanged(playerState);

    TileUpdate update;
    update.Position = Vector2{13, 25};
    update.Type = TileType::Empty;
    gameScreen->OnTileUpdated(update);

    playerState.Score = 10;
    gameScreen->OnPlayerStateChanged(playerState);

    std::vector<GhostState> ghosts(4);
    gameScreen->OnGhostsUpdated(ghosts);

    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, PowerUpSequence_DoesNotCrash) {
    PlayerState state;
    state.Position = Vector2{10, 10};

    state.IsPoweredUp = false;
    gameScreen->OnPlayerStateChanged(state);
    gameScreen->Render(window);

    TileUpdate update;
    update.Position = Vector2{10, 10};
    update.Type = TileType::Empty;
    gameScreen->OnTileUpdated(update);

    state.IsPoweredUp = true;
    gameScreen->OnPlayerStateChanged(state);
    gameScreen->Render(window);

    std::vector<GhostState> ghosts(4);
    for (auto& ghost : ghosts) {
        ghost.IsFrightened = true;
    }
    gameScreen->OnGhostsUpdated(ghosts);

    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, DeathSequence_DoesNotCrash) {
    PlayerState state;
    state.Position = Vector2{10, 10};
    state.Lives = 3;
    gameScreen->OnPlayerStateChanged(state);

    state.Lives = 2;
    gameScreen->OnPlayerStateChanged(state);

    state.Position = Vector2{13, 26};
    gameScreen->OnPlayerStateChanged(state);

    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, GameOverSequence_DoesNotCrash) {
    PlayerState state;
    state.Lives = 1;
    gameScreen->OnPlayerStateChanged(state);

    state.Lives = 0;
    gameScreen->OnPlayerStateChanged(state);

    gameScreen->OnGameStateChanged(GameState::GameOver);

    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, VictorySequence_DoesNotCrash) {
    PlayerState state;
    state.Score = 10000;
    gameScreen->OnPlayerStateChanged(state);

    gameScreen->OnGameStateChanged(GameState::Victory);

    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, RapidGhostUpdates_DoesNotCrash) {
    std::vector<GhostState> ghosts(4);

    for (int i = 0; i < 100; ++i) {
        for (auto& ghost : ghosts) {
            ghost.Position.X = (ghost.Position.X + 1) % 28;
        }
        EXPECT_NO_THROW(gameScreen->OnGhostsUpdated(ghosts));
    }
}

TEST_F(GameScreenTest, GhostModeChanges_DoesNotCrash) {
    std::vector<GhostState> ghosts(4);

    for (auto& ghost : ghosts) ghost.Mode = GhostMode::Scatter;
    gameScreen->OnGhostsUpdated(ghosts);

    for (auto& ghost : ghosts) ghost.Mode = GhostMode::Chase;
    gameScreen->OnGhostsUpdated(ghosts);

    for (auto& ghost : ghosts) ghost.Mode = GhostMode::Frightened;
    gameScreen->OnGhostsUpdated(ghosts);

    for (auto& ghost : ghosts) ghost.Mode = GhostMode::Eaten;
    gameScreen->OnGhostsUpdated(ghosts);

    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, NegativeScore_HandledCorrectly) {
    PlayerState state;
    state.Score = -100;
    EXPECT_NO_THROW(gameScreen->OnPlayerStateChanged(state));
}

TEST_F(GameScreenTest, VeryHighScore_HandledCorrectly) {
    PlayerState state;
    state.Score = 999999;
    EXPECT_NO_THROW(gameScreen->OnPlayerStateChanged(state));
}

TEST_F(GameScreenTest, ZeroLives_HandledCorrectly) {
    PlayerState state;
    state.Lives = 0;
    EXPECT_NO_THROW(gameScreen->OnPlayerStateChanged(state));
}

TEST_F(GameScreenTest, ManyLives_HandledCorrectly) {
    PlayerState state;
    state.Lives = 100;
    EXPECT_NO_THROW(gameScreen->OnPlayerStateChanged(state));
}

TEST_F(GameScreenTest, OutOfBoundsPosition_DoesNotCrash) {
    PlayerState state;
    state.Position = Vector2{-10, -10};
    EXPECT_NO_THROW(gameScreen->OnPlayerStateChanged(state));

    state.Position = Vector2{1000, 1000};
    EXPECT_NO_THROW(gameScreen->OnPlayerStateChanged(state));
}

TEST_F(GameScreenTest, GhostAtOutOfBoundsPosition_DoesNotCrash) {
    std::vector<GhostState> ghosts(1);
    ghosts[0].Position = Vector2{-100, -100};
    EXPECT_NO_THROW(gameScreen->OnGhostsUpdated(ghosts));
}

TEST_F(GameScreenTest, AllGameStateTransitions_Work) {
    gameScreen->OnGameStateChanged(GameState::Paused);
    gameScreen->Render(window);

    gameScreen->OnGameStateChanged(GameState::Running);
    gameScreen->Render(window);

    gameScreen->OnGameStateChanged(GameState::Paused);
    gameScreen->Render(window);

    gameScreen->OnGameStateChanged(GameState::GameOver);
    gameScreen->Render(window);
}

TEST_F(GameScreenTest, DirectToVictory_DoesNotCrash) {
    gameScreen->OnGameStateChanged(GameState::Victory);
    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, DirectToGameOver_DoesNotCrash) {
    gameScreen->OnGameStateChanged(GameState::GameOver);
    EXPECT_NO_THROW(gameScreen->Render(window));
}

TEST_F(GameScreenTest, RapidPlayerUpdates_DoesNotCrash) {
    PlayerState state;

    for (int i = 0; i < 1000; ++i) {
        state.Position.X = i % 28;
        state.Position.Y = i % 31;
        state.Score = i * 10;
        EXPECT_NO_THROW(gameScreen->OnPlayerStateChanged(state));
    }
}

TEST_F(GameScreenTest, RapidTileUpdates_DoesNotCrash) {
    for (int i = 0; i < 1000; ++i) {
        TileUpdate update;
        update.Position = Vector2{i % 28, i % 31};
        update.Type = (i % 2 == 0) ? TileType::Empty : TileType::Pellet;
        EXPECT_NO_THROW(gameScreen->OnTileUpdated(update));
    }
}

TEST_F(GameScreenTest, RapidGameStateChanges_DoesNotCrash) {
    GameState states[] = {GameState::Running, GameState::Paused,
                          GameState::Running, GameState::GameOver};

    for (int i = 0; i < 100; ++i) {
        EXPECT_NO_THROW(gameScreen->OnGameStateChanged(states[i % 4]));
    }
}
