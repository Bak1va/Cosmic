#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "InputController.hpp"
#include "IGameEngine.hpp"

using namespace Pacman;
using ::testing::_;

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

class InputControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockEngine = std::make_shared<MockGameEngine>();
        controller = std::make_unique<InputController>(mockEngine);
    }

    std::shared_ptr<MockGameEngine> mockEngine;
    std::unique_ptr<InputController> controller;
};

TEST_F(InputControllerTest, UpArrowKey_CallsSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Up;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Up)).Times(1);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, DownArrowKey_CallsSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Down;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Down)).Times(1);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, LeftArrowKey_CallsSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Left;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Left)).Times(1);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, RightArrowKey_CallsSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Right;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Right)).Times(1);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, WKey_CallsSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::W;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Up)).Times(1);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, SKey_CallsSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::S;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Down)).Times(1);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, AKey_CallsSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::A;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Left)).Times(1);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, DKey_CallsSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::D;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Right)).Times(1);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, MultipleKeyPresses_CallSetPlayerDirectionEachTime) {
    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(4);

    sf::Event event1;
    event1.type = sf::Event::KeyPressed;
    event1.key.code = sf::Keyboard::Up;
    controller->ProcessEvent(event1);

    sf::Event event2;
    event2.type = sf::Event::KeyPressed;
    event2.key.code = sf::Keyboard::Down;
    controller->ProcessEvent(event2);

    sf::Event event3;
    event3.type = sf::Event::KeyPressed;
    event3.key.code = sf::Keyboard::Left;
    controller->ProcessEvent(event3);

    sf::Event event4;
    event4.type = sf::Event::KeyPressed;
    event4.key.code = sf::Keyboard::Right;
    controller->ProcessEvent(event4);
}

TEST_F(InputControllerTest, KeyRelease_DoesNotCallSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyReleased;
    event.key.code = sf::Keyboard::Up;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(0);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, SpaceKey_DoesNotCallSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Space;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(0);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, EscapeKey_DoesNotCallSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Escape;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(0);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, NumberKey_DoesNotCallSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Num1;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(0);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, LetterKey_NotWASD_DoesNotCallSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Z;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(0);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, MouseButtonEvent_DoesNotCallSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::MouseButtonPressed;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(0);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, MouseMoveEvent_DoesNotCallSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::MouseMoved;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(0);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, WindowClosedEvent_DoesNotCallSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::Closed;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(0);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, WindowResizedEvent_DoesNotCallSetPlayerDirection) {
    sf::Event event;
    event.type = sf::Event::Resized;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(0);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, RapidKeyPresses_AllProcessed) {
    EXPECT_CALL(*mockEngine, SetPlayerDirection(_)).Times(10);

    for (int i = 0; i < 10; ++i) {
        sf::Event event;
        event.type = sf::Event::KeyPressed;
        event.key.code = (i % 2 == 0) ? sf::Keyboard::Up : sf::Keyboard::Down;
        controller->ProcessEvent(event);
    }
}

TEST_F(InputControllerTest, EngineReferenceKept_CallsCorrectEngine) {
    auto engine1 = std::make_shared<MockGameEngine>();
    auto engine2 = std::make_shared<MockGameEngine>();

    auto controller1 = std::make_unique<InputController>(engine1);
    auto controller2 = std::make_unique<InputController>(engine2);

    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Up;

    EXPECT_CALL(*engine1, SetPlayerDirection(Direction::Up)).Times(1);
    EXPECT_CALL(*engine2, SetPlayerDirection(_)).Times(0);

    controller1->ProcessEvent(event);
}

TEST_F(InputControllerTest, UpperCaseW_SameAsLowerCase) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::W;

    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Up)).Times(1);
    controller->ProcessEvent(event);
}

TEST_F(InputControllerTest, UpKeys_MapToUpDirection) {
    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Up)).Times(2);

    sf::Event event1;
    event1.type = sf::Event::KeyPressed;
    event1.key.code = sf::Keyboard::Up;
    controller->ProcessEvent(event1);

    sf::Event event2;
    event2.type = sf::Event::KeyPressed;
    event2.key.code = sf::Keyboard::W;
    controller->ProcessEvent(event2);
}

TEST_F(InputControllerTest, DownKeys_MapToDownDirection) {
    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Down)).Times(2);

    sf::Event event1;
    event1.type = sf::Event::KeyPressed;
    event1.key.code = sf::Keyboard::Down;
    controller->ProcessEvent(event1);

    sf::Event event2;
    event2.type = sf::Event::KeyPressed;
    event2.key.code = sf::Keyboard::S;
    controller->ProcessEvent(event2);
}

TEST_F(InputControllerTest, LeftKeys_MapToLeftDirection) {
    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Left)).Times(2);

    sf::Event event1;
    event1.type = sf::Event::KeyPressed;
    event1.key.code = sf::Keyboard::Left;
    controller->ProcessEvent(event1);

    sf::Event event2;
    event2.type = sf::Event::KeyPressed;
    event2.key.code = sf::Keyboard::A;
    controller->ProcessEvent(event2);
}

TEST_F(InputControllerTest, RightKeys_MapToRightDirection) {
    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Right)).Times(2);

    sf::Event event1;
    event1.type = sf::Event::KeyPressed;
    event1.key.code = sf::Keyboard::Right;
    controller->ProcessEvent(event1);

    sf::Event event2;
    event2.type = sf::Event::KeyPressed;
    event2.key.code = sf::Keyboard::D;
    controller->ProcessEvent(event2);
}

TEST_F(InputControllerTest, AlternatingArrowAndWASD_BothWork) {
    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Up)).Times(2);
    EXPECT_CALL(*mockEngine, SetPlayerDirection(Direction::Left)).Times(2);

    sf::Event event1;
    event1.type = sf::Event::KeyPressed;
    event1.key.code = sf::Keyboard::Up;
    controller->ProcessEvent(event1);

    sf::Event event2;
    event2.type = sf::Event::KeyPressed;
    event2.key.code = sf::Keyboard::W;
    controller->ProcessEvent(event2);

    sf::Event event3;
    event3.type = sf::Event::KeyPressed;
    event3.key.code = sf::Keyboard::Left;
    controller->ProcessEvent(event3);

    sf::Event event4;
    event4.type = sf::Event::KeyPressed;
    event4.key.code = sf::Keyboard::A;
    controller->ProcessEvent(event4);
}
