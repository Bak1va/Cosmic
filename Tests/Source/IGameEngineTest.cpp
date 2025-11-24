#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "IGameEngine.hpp"
#include "IEventListener.hpp"

using namespace Pacman;
using ::testing::_;

class MockEventListener : public IEventListener {
public:
    MOCK_METHOD(void, OnTileUpdated, (const TileUpdate& update), (override));
    MOCK_METHOD(void, OnPlayerStateChanged, (const PlayerState& state), (override));
    MOCK_METHOD(void, OnGameStateChanged, (GameState state), (override));
    MOCK_METHOD(void, OnGhostsUpdated, (const std::vector<GhostState>& ghosts), (override));
    MOCK_METHOD(void, OnGhostModeChanged, (GhostMode mode), (override));
};

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

class IGameEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockEngine = std::make_unique<MockGameEngine>();
        mockListener = std::make_shared<MockEventListener>();
    }

    std::unique_ptr<MockGameEngine> mockEngine;
    std::shared_ptr<MockEventListener> mockListener;
};

TEST_F(IGameEngineTest, StartNewGame_CanBeCalled) {
    EXPECT_CALL(*mockEngine, StartNewGame()).Times(1);
    mockEngine->StartNewGame();
}

TEST_F(IGameEngineTest, StartNewGame_MultipleCalls) {
    EXPECT_CALL(*mockEngine, StartNewGame()).Times(3);

    for (int i = 0; i < 3; ++i) {
        mockEngine->StartNewGame();
    }
}

TEST_F(IGameEngineTest, Update_WithPositiveDeltaTime) {
    EXPECT_CALL(*mockEngine, Update(0.016f)).Times(1);
    mockEngine->Update(0.016f);
}

TEST_F(IGameEngineTest, Update_WithZeroDeltaTime) {
    EXPECT_CALL(*mockEngine, Update(0.0f)).Times(1);
    mockEngine->Update(0.0f);
}

TEST_F(IGameEngineTest, Update_WithLargeDeltaTime) {
    EXPECT_CALL(*mockEngine, Update(1.0f)).Times(1);
    mockEngine->Update(1.0f);
}

TEST_F(IGameEngineTest, Update_MultipleCalls) {
    EXPECT_CALL(*mockEngine, Update(_)).Times(60);

    for (int i = 0; i < 60; ++i) {
        mockEngine->Update(0.016f);
    }
}

TEST_F(IGameEngineTest, Update_VaryingDeltaTimes) {
    EXPECT_CALL(*mockEngine, Update(0.016f)).Times(1);
    EXPECT_CALL(*mockEngine, Update(0.032f)).Times(1);
    EXPECT_CALL(*mockEngine, Update(0.008f)).Times(1);

    mockEngine->Update(0.016f);
    mockEngine->Update(0.032f);
    mockEngine->Update(0.008f);
}

TEST_F(IGameEngineTest, SetPaused_True) {
    EXPECT_CALL(*mockEngine, SetPaused(true)).Times(1);
    mockEngine->SetPaused(true);
}

TEST_F(IGameEngineTest, SetPaused_False) {
    EXPECT_CALL(*mockEngine, SetPaused(false)).Times(1);
    mockEngine->SetPaused(false);
}

TEST_F(IGameEngineTest, SetPaused_Toggle) {
    EXPECT_CALL(*mockEngine, SetPaused(true)).Times(1);
    EXPECT_CALL(*mockEngine, SetPaused(false)).Times(1);

    mockEngine->SetPaused(true);
    mockEngine->SetPaused(false);
}

TEST_F(IGameEngineTest, SetPaused_MultipleTrues) {
    EXPECT_CALL(*mockEngine, SetPaused(true)).Times(3);

    for (int i = 0; i < 3; ++i) {
        mockEngine->SetPaused(true);
    }
}

TEST_F(IGameEngineTest, SetPlayerDirection_AllDirections) {
    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Up)).Times(1);
    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Down)).Times(1);
    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Left)).Times(1);
    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Right)).Times(1);
    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::None)).Times(1);

    mockEngine->SetPlayerDirection(Direction::Up);
    mockEngine->SetPlayerDirection(Direction::Down);
    mockEngine->SetPlayerDirection(Direction::Left);
    mockEngine->SetPlayerDirection(Direction::Right);
    mockEngine->SetPlayerDirection(Direction::None);
}

TEST_F(IGameEngineTest, SetPlayerDirection_RapidChanges) {
    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(100);

    Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};
    for (int i = 0; i < 100; ++i) {
        mockEngine->SetPlayerDirection(dirs[i % 4]);
    }
}

TEST_F(IGameEngineTest, GetState_ReturnsGameState) {
    EXPECT_CALL(*mockEngine, GetState())
        .WillOnce(testing::Return(GameState::Running));

    GameState state = mockEngine->GetState();
    EXPECT_EQ(state, GameState::Running);
}

TEST_F(IGameEngineTest, GetState_AllStates) {
    for (auto state : {GameState::Running, GameState::Paused,
                        GameState::GameOver, GameState::Victory}) {
        auto engine = std::make_unique<MockGameEngine>();
        EXPECT_CALL(*engine, GetState())
            .WillOnce(testing::Return(state));

        EXPECT_EQ(engine->GetState(), state);
    }
}

TEST_F(IGameEngineTest, GetState_IsConst) {
    const IGameEngine* constEngine = mockEngine.get();
    EXPECT_CALL(*mockEngine, GetState())
        .WillOnce(testing::Return(GameState::Paused));

    constEngine->GetState();
}

TEST_F(IGameEngineTest, GetPlayerState_ReturnsPlayerState) {
    PlayerState expectedState;
    expectedState.Position = Vector2{13, 26};
    expectedState.Score = 1000;
    expectedState.Lives = 3;

    EXPECT_CALL(*mockEngine, GetPlayerState())
        .WillOnce(testing::Return(expectedState));

    PlayerState state = mockEngine->GetPlayerState();
    EXPECT_EQ(state.Position, expectedState.Position);
    EXPECT_EQ(state.Score, expectedState.Score);
    EXPECT_EQ(state.Lives, expectedState.Lives);
}

TEST_F(IGameEngineTest, GetPlayerState_IsConst) {
    const IGameEngine* constEngine = mockEngine.get();
    PlayerState state;

    EXPECT_CALL(*mockEngine, GetPlayerState())
        .WillOnce(testing::Return(state));

    constEngine->GetPlayerState();
}

TEST_F(IGameEngineTest, GetMapSize_ReturnsVector2) {
    EXPECT_CALL(*mockEngine, GetMapSize())
        .WillOnce(testing::Return(Vector2{28, 31}));

    Vector2 size = mockEngine->GetMapSize();
    EXPECT_EQ(size.X, 28);
    EXPECT_EQ(size.Y, 31);
}

TEST_F(IGameEngineTest, GetMapSize_IsConst) {
    const IGameEngine* constEngine = mockEngine.get();

    EXPECT_CALL(*mockEngine, GetMapSize())
        .WillOnce(testing::Return(Vector2{28, 31}));

    constEngine->GetMapSize();
}

TEST_F(IGameEngineTest, GetTileAt_ReturnsTileType) {
    Vector2 pos{5, 5};

    EXPECT_CALL(*mockEngine, GetTileAt(pos))
        .WillOnce(testing::Return(TileType::Pellet));

    TileType tile = mockEngine->GetTileAt(pos);
    EXPECT_EQ(tile, TileType::Pellet);
}

TEST_F(IGameEngineTest, GetTileAt_DifferentPositions) {
    EXPECT_CALL(*mockEngine, GetTileAt(Vector2{0, 0}))
        .WillOnce(testing::Return(TileType::Wall));
    EXPECT_CALL(*mockEngine, GetTileAt(Vector2{1, 1}))
        .WillOnce(testing::Return(TileType::Pellet));
    EXPECT_CALL(*mockEngine, GetTileAt(Vector2{13, 14}))
        .WillOnce(testing::Return(TileType::Path));

    mockEngine->GetTileAt({0, 0});
    mockEngine->GetTileAt({1, 1});
    mockEngine->GetTileAt({13, 14});
}

TEST_F(IGameEngineTest, GetTileAt_IsConst) {
    const IGameEngine* constEngine = mockEngine.get();

    EXPECT_CALL(*mockEngine, GetTileAt(_))
        .WillOnce(testing::Return(TileType::Path));

    constEngine->GetTileAt({5, 5});
}

TEST_F(IGameEngineTest, GetPelletPositions_ReturnsVector) {
    std::vector<Vector2> positions = {{1, 1}, {2, 1}, {3, 1}};

    EXPECT_CALL(*mockEngine, GetPelletPositions())
        .WillOnce(testing::Return(positions));

    auto result = mockEngine->GetPelletPositions();
    EXPECT_EQ(result.size(), 3);
}

TEST_F(IGameEngineTest, GetPelletPositions_CanBeEmpty) {
    std::vector<Vector2> positions;

    EXPECT_CALL(*mockEngine, GetPelletPositions())
        .WillOnce(testing::Return(positions));

    auto result = mockEngine->GetPelletPositions();
    EXPECT_TRUE(result.empty());
}

TEST_F(IGameEngineTest, GetPelletPositions_IsConst) {
    const IGameEngine* constEngine = mockEngine.get();
    std::vector<Vector2> positions;

    EXPECT_CALL(*mockEngine, GetPelletPositions())
        .WillOnce(testing::Return(positions));

    constEngine->GetPelletPositions();
}

TEST_F(IGameEngineTest, GetPelletCount_ReturnsCount) {
    EXPECT_CALL(*mockEngine, GetPelletCount())
        .WillOnce(testing::Return(244));

    int count = mockEngine->GetPelletCount();
    EXPECT_EQ(count, 244);
}

TEST_F(IGameEngineTest, GetPelletCount_CanBeZero) {
    EXPECT_CALL(*mockEngine, GetPelletCount())
        .WillOnce(testing::Return(0));

    int count = mockEngine->GetPelletCount();
    EXPECT_EQ(count, 0);
}

TEST_F(IGameEngineTest, GetPelletCount_IsConst) {
    const IGameEngine* constEngine = mockEngine.get();

    EXPECT_CALL(*mockEngine, GetPelletCount())
        .WillOnce(testing::Return(100));

    constEngine->GetPelletCount();
}

TEST_F(IGameEngineTest, GetGhostStates_ReturnsVector) {
    std::vector<GhostState> ghosts(4);

    EXPECT_CALL(*mockEngine, GetGhostStates())
        .WillOnce(testing::Return(ghosts));

    auto result = mockEngine->GetGhostStates();
    EXPECT_EQ(result.size(), 4);
}

TEST_F(IGameEngineTest, GetGhostStates_CanBeEmpty) {
    std::vector<GhostState> ghosts;

    EXPECT_CALL(*mockEngine, GetGhostStates())
        .WillOnce(testing::Return(ghosts));

    auto result = mockEngine->GetGhostStates();
    EXPECT_TRUE(result.empty());
}

TEST_F(IGameEngineTest, GetGhostStates_IsConst) {
    const IGameEngine* constEngine = mockEngine.get();
    std::vector<GhostState> ghosts;

    EXPECT_CALL(*mockEngine, GetGhostStates())
        .WillOnce(testing::Return(ghosts));

    constEngine->GetGhostStates();
}

TEST_F(IGameEngineTest, GetGlobalGhostMode_ReturnsMode) {
    EXPECT_CALL(*mockEngine, GetGlobalGhostMode())
        .WillOnce(testing::Return(GhostMode::Chase));

    GhostMode mode = mockEngine->GetGlobalGhostMode();
    EXPECT_EQ(mode, GhostMode::Chase);
}

TEST_F(IGameEngineTest, GetGlobalGhostMode_AllModes) {
    for (auto mode : {GhostMode::Chase, GhostMode::Scatter,
                       GhostMode::Frightened, GhostMode::Eaten}) {
        auto engine = std::make_unique<MockGameEngine>();
        EXPECT_CALL(*engine, GetGlobalGhostMode())
            .WillOnce(testing::Return(mode));

        EXPECT_EQ(engine->GetGlobalGhostMode(), mode);
    }
}

TEST_F(IGameEngineTest, GetGlobalGhostMode_IsConst) {
    const IGameEngine* constEngine = mockEngine.get();

    EXPECT_CALL(*mockEngine, GetGlobalGhostMode())
        .WillOnce(testing::Return(GhostMode::Scatter));

    constEngine->GetGlobalGhostMode();
}

TEST_F(IGameEngineTest, AddListener_AcceptsListener) {
    EXPECT_CALL(*mockEngine, AddListener(mockListener)).Times(1);
    mockEngine->AddListener(mockListener);
}

TEST_F(IGameEngineTest, AddListener_MultipleListeners) {
    auto listener1 = std::make_shared<MockEventListener>();
    auto listener2 = std::make_shared<MockEventListener>();

    EXPECT_CALL(*mockEngine, AddListener(listener1)).Times(1);
    EXPECT_CALL(*mockEngine, AddListener(listener2)).Times(1);

    mockEngine->AddListener(listener1);
    mockEngine->AddListener(listener2);
}

TEST_F(IGameEngineTest, AddListener_SameListenerTwice) {
    EXPECT_CALL(*mockEngine, AddListener(mockListener)).Times(2);

    mockEngine->AddListener(mockListener);
    mockEngine->AddListener(mockListener);
}

TEST_F(IGameEngineTest, AddListener_NullListener) {
    EXPECT_CALL(*mockEngine, AddListener(nullptr)).Times(1);
    mockEngine->AddListener(nullptr);
}

TEST_F(IGameEngineTest, RemoveListener_RemovesListener) {
    EXPECT_CALL(*mockEngine, RemoveListener(mockListener)).Times(1);
    mockEngine->RemoveListener(mockListener);
}

TEST_F(IGameEngineTest, RemoveListener_NonExistentListener) {
    auto listener = std::make_shared<MockEventListener>();
    EXPECT_CALL(*mockEngine, RemoveListener(listener)).Times(1);
    mockEngine->RemoveListener(listener);
}

TEST_F(IGameEngineTest, RemoveListener_NullListener) {
    EXPECT_CALL(*mockEngine, RemoveListener(nullptr)).Times(1);
    mockEngine->RemoveListener(nullptr);
}

TEST_F(IGameEngineTest, AddThenRemoveListener) {
    EXPECT_CALL(*mockEngine, AddListener(mockListener)).Times(1);
    EXPECT_CALL(*mockEngine, RemoveListener(mockListener)).Times(1);

    mockEngine->AddListener(mockListener);
    mockEngine->RemoveListener(mockListener);
}

TEST_F(IGameEngineTest, AddMultipleThenRemoveAll) {
    auto listener1 = std::make_shared<MockEventListener>();
    auto listener2 = std::make_shared<MockEventListener>();

    EXPECT_CALL(*mockEngine, AddListener(_)).Times(2);
    EXPECT_CALL(*mockEngine, RemoveListener(_)).Times(2);

    mockEngine->AddListener(listener1);
    mockEngine->AddListener(listener2);
    mockEngine->RemoveListener(listener1);
    mockEngine->RemoveListener(listener2);
}

TEST_F(IGameEngineTest, VirtualDestructor_AllowsPolymorphicDeletion) {
    IGameEngine* engine = new MockGameEngine();
    EXPECT_NO_THROW(delete engine);
}

TEST_F(IGameEngineTest, CreateGameEngine_ReturnsNonNull) {
    auto engine = CreateGameEngine();
    EXPECT_NE(engine, nullptr);
}

TEST_F(IGameEngineTest, CreateGameEngine_ReturnsIGameEngine) {
    auto engine = CreateGameEngine();
    IGameEngine* ptr = engine.get();
    EXPECT_NE(ptr, nullptr);
}

TEST_F(IGameEngineTest, CreateGameEngine_MultipleInstances) {
    auto engine1 = CreateGameEngine();
    auto engine2 = CreateGameEngine();

    EXPECT_NE(engine1, nullptr);
    EXPECT_NE(engine2, nullptr);
    EXPECT_NE(engine1.get(), engine2.get());
}

TEST_F(IGameEngineTest, CompleteGameFlow) {
    testing::InSequence seq;

    EXPECT_CALL(*mockEngine, StartNewGame()).Times(1);
    EXPECT_CALL(*mockEngine, GetState())
        .WillOnce(testing::Return(GameState::Running));
    EXPECT_CALL(*mockEngine, Update(_)).Times(1);
    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Right)).Times(1);
    EXPECT_CALL(*mockEngine, GetPlayerState())
        .WillOnce(testing::Return(PlayerState{}));
    EXPECT_CALL(*mockEngine, GetGhostStates())
        .WillOnce(testing::Return(std::vector<GhostState>{}));
    EXPECT_CALL(*mockEngine, SetPaused(true)).Times(1);

    mockEngine->StartNewGame();
    mockEngine->GetState();
    mockEngine->Update(0.016f);
    mockEngine->SetPlayerDirection(Direction::Right);
    mockEngine->GetPlayerState();
    mockEngine->GetGhostStates();
    mockEngine->SetPaused(true);
}

TEST_F(IGameEngineTest, GameLoopSimulation) {
    EXPECT_CALL(*mockEngine, StartNewGame()).Times(1);
    EXPECT_CALL(*mockEngine, GetState())
        .WillRepeatedly(testing::Return(GameState::Running));
    EXPECT_CALL(*mockEngine, Update(_)).Times(60);

    mockEngine->StartNewGame();

    for (int i = 0; i < 60; ++i) {
        if (mockEngine->GetState() == GameState::Running) {
            mockEngine->Update(0.016f);
        }
    }
}

TEST_F(IGameEngineTest, UpdateWithNegativeDeltaTime) {
    EXPECT_CALL(*mockEngine, Update(-0.016f)).Times(1);
    mockEngine->Update(-0.016f);
}

TEST_F(IGameEngineTest, UpdateWithVerySmallDeltaTime) {
    EXPECT_CALL(*mockEngine, Update(0.0001f)).Times(1);
    mockEngine->Update(0.0001f);
}

TEST_F(IGameEngineTest, UpdateWithVeryLargeDeltaTime) {
    EXPECT_CALL(*mockEngine, Update(10.0f)).Times(1);
    mockEngine->Update(10.0f);
}

TEST_F(IGameEngineTest, GetTileAt_OutOfBounds) {
    EXPECT_CALL(*mockEngine, GetTileAt(Vector2{-1, -1}))
        .WillOnce(testing::Return(TileType::Wall));
    EXPECT_CALL(*mockEngine, GetTileAt(Vector2{100, 100}))
        .WillOnce(testing::Return(TileType::Wall));

    mockEngine->GetTileAt({-1, -1});
    mockEngine->GetTileAt({100, 100});
}

TEST_F(IGameEngineTest, RapidDirectionChanges) {
    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(1000);

    Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};
    for (int i = 0; i < 1000; ++i) {
        mockEngine->SetPlayerDirection(dirs[i % 4]);
    }
}

TEST_F(IGameEngineTest, RapidPauseToggle) {
    EXPECT_CALL(*mockEngine, SetPaused(_)).Times(100);

    for (int i = 0; i < 100; ++i) {
        mockEngine->SetPaused(i % 2 == 0);
    }
}

TEST_F(IGameEngineTest, ManyListenerOperations) {
    EXPECT_CALL(*mockEngine, AddListener(_)).Times(50);
    EXPECT_CALL(*mockEngine, RemoveListener(_)).Times(50);

    std::vector<std::shared_ptr<MockEventListener>> listeners;
    for (int i = 0; i < 50; ++i) {
        auto listener = std::make_shared<MockEventListener>();
        listeners.push_back(listener);
        mockEngine->AddListener(listener);
    }

    for (auto& listener : listeners) {
        mockEngine->RemoveListener(listener);
    }
}
