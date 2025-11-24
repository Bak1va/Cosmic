#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MenuScreen.hpp"
#include "IMenuListener.hpp"

using namespace Pacman;

class MockMenuListener : public IMenuListener {
public:
    MOCK_METHOD(void, OnPlaySelected, (), (override));
    MOCK_METHOD(void, OnQuitSelected, (), (override));
};

class MenuScreenTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockListener = std::make_shared<MockMenuListener>();
        menuScreen = std::make_unique<MenuScreen>(mockListener);
        window.create(sf::VideoMode(800, 600), "Test", sf::Style::None);
    }

    void TearDown() override {
        window.close();
    }

    std::shared_ptr<MockMenuListener> mockListener;
    std::unique_ptr<MenuScreen> menuScreen;
    sf::RenderWindow window;
};

TEST_F(MenuScreenTest, Construction_WithListener_DoesNotCrash) {
    EXPECT_NE(menuScreen, nullptr);
}

TEST_F(MenuScreenTest, Construction_InitiallyNotClosed) {
    EXPECT_FALSE(menuScreen->ShouldClose());
}

TEST_F(MenuScreenTest, DownKey_ChangesSelection) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Down;

    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, UpKey_ChangesSelection) {
    sf::Event downEvent;
    downEvent.type = sf::Event::KeyPressed;
    downEvent.key.code = sf::Keyboard::Down;
    menuScreen->HandleEvent(downEvent);

    sf::Event upEvent;
    upEvent.type = sf::Event::KeyPressed;
    upEvent.key.code = sf::Keyboard::Up;
    menuScreen->HandleEvent(upEvent);
}

TEST_F(MenuScreenTest, DownKey_AtBottom_WrapsToTop) {
    sf::Event downEvent;
    downEvent.type = sf::Event::KeyPressed;
    downEvent.key.code = sf::Keyboard::Down;
    menuScreen->HandleEvent(downEvent);

    menuScreen->HandleEvent(downEvent);
}

TEST_F(MenuScreenTest, UpKey_AtTop_WrapsToBottom) {
    sf::Event upEvent;
    upEvent.type = sf::Event::KeyPressed;
    upEvent.key.code = sf::Keyboard::Up;
    menuScreen->HandleEvent(upEvent);
}

TEST_F(MenuScreenTest, SKey_ChangesSelectionDown) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::S;

    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, WKey_ChangesSelectionUp) {
    sf::Event downEvent;
    downEvent.type = sf::Event::KeyPressed;
    downEvent.key.code = sf::Keyboard::Down;
    menuScreen->HandleEvent(downEvent);

    sf::Event wEvent;
    wEvent.type = sf::Event::KeyPressed;
    wEvent.key.code = sf::Keyboard::W;
    menuScreen->HandleEvent(wEvent);
}

TEST_F(MenuScreenTest, EnterKey_OnPlay_CallsOnPlaySelected) {
    EXPECT_CALL(*mockListener, OnPlaySelected()).Times(1);

    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Enter;
    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, EnterKey_OnQuit_CallsOnQuitSelected) {
    sf::Event downEvent;
    downEvent.type = sf::Event::KeyPressed;
    downEvent.key.code = sf::Keyboard::Down;
    menuScreen->HandleEvent(downEvent);

    EXPECT_CALL(*mockListener, OnQuitSelected()).Times(1);

    sf::Event enterEvent;
    enterEvent.type = sf::Event::KeyPressed;
    enterEvent.key.code = sf::Keyboard::Enter;
    menuScreen->HandleEvent(enterEvent);
}

TEST_F(MenuScreenTest, SpaceKey_OnPlay_CallsOnPlaySelected) {
    EXPECT_CALL(*mockListener, OnPlaySelected()).Times(1);

    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Space;
    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, SpaceKey_OnQuit_CallsOnQuitSelected) {
    sf::Event downEvent;
    downEvent.type = sf::Event::KeyPressed;
    downEvent.key.code = sf::Keyboard::Down;
    menuScreen->HandleEvent(downEvent);

    EXPECT_CALL(*mockListener, OnQuitSelected()).Times(1);

    sf::Event spaceEvent;
    spaceEvent.type = sf::Event::KeyPressed;
    spaceEvent.key.code = sf::Keyboard::Space;
    menuScreen->HandleEvent(spaceEvent);
}

TEST_F(MenuScreenTest, MultipleEnterPresses_CallsListenerMultipleTimes) {
    EXPECT_CALL(*mockListener, OnPlaySelected()).Times(3);

    for (int i = 0; i < 3; ++i) {
        sf::Event event;
        event.type = sf::Event::KeyPressed;
        event.key.code = sf::Keyboard::Enter;
        menuScreen->HandleEvent(event);
    }
}

TEST_F(MenuScreenTest, MouseClick_OnPlay_CallsOnPlaySelected) {
    menuScreen->Render(window);

    EXPECT_CALL(*mockListener, OnPlaySelected()).Times(1);

    sf::Event event;
    event.type = sf::Event::MouseButtonPressed;
    event.mouseButton.button = sf::Mouse::Left;
    event.mouseButton.x = 400;
    event.mouseButton.y = 250;

    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, MouseClick_OnQuit_CallsOnQuitSelected) {
    menuScreen->Render(window);

    EXPECT_CALL(*mockListener, OnQuitSelected()).Times(1);

    sf::Event event;
    event.type = sf::Event::MouseButtonPressed;
    event.mouseButton.button = sf::Mouse::Left;
    event.mouseButton.x = 400;
    event.mouseButton.y = 350;

    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, RightMouseClick_DoesNotTriggerSelection) {
    menuScreen->Render(window);

    EXPECT_CALL(*mockListener, OnPlaySelected()).Times(0);
    EXPECT_CALL(*mockListener, OnQuitSelected()).Times(0);

    sf::Event event;
    event.type = sf::Event::MouseButtonPressed;
    event.mouseButton.button = sf::Mouse::Right;
    event.mouseButton.x = 400;
    event.mouseButton.y = 250;

    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, MouseMove_OverPlay_ChangesSelection) {
    menuScreen->Render(window);

    sf::Event event;
    event.type = sf::Event::MouseMoved;
    event.mouseMove.x = 400;
    event.mouseMove.y = 250;

    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, MouseMove_OverQuit_ChangesSelection) {
    menuScreen->Render(window);

    sf::Event event;
    event.type = sf::Event::MouseMoved;
    event.mouseMove.x = 400;
    event.mouseMove.y = 350;

    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, MouseMove_OutsideButtons_NoChange) {
    menuScreen->Render(window);

    sf::Event event;
    event.type = sf::Event::MouseMoved;
    event.mouseMove.x = 10;
    event.mouseMove.y = 10;

    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, EscapeKey_RequestsClose) {
    EXPECT_FALSE(menuScreen->ShouldClose());

    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Escape;
    menuScreen->HandleEvent(event);

    EXPECT_TRUE(menuScreen->ShouldClose());
}

TEST_F(MenuScreenTest, MultipleEscapePresses_KeepsClosedState) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Escape;

    menuScreen->HandleEvent(event);
    menuScreen->HandleEvent(event);

    EXPECT_TRUE(menuScreen->ShouldClose());
}

TEST_F(MenuScreenTest, WindowClosedEvent_RequestsClose) {
    sf::Event event;
    event.type = sf::Event::Closed;
    menuScreen->HandleEvent(event);

    EXPECT_TRUE(menuScreen->ShouldClose());
}

TEST_F(MenuScreenTest, KeyRelease_DoesNotTriggerSelection) {
    EXPECT_CALL(*mockListener, OnPlaySelected()).Times(0);

    sf::Event event;
    event.type = sf::Event::KeyReleased;
    event.key.code = sf::Keyboard::Enter;
    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, RandomKey_DoesNotTriggerSelection) {
    EXPECT_CALL(*mockListener, OnPlaySelected()).Times(0);
    EXPECT_CALL(*mockListener, OnQuitSelected()).Times(0);

    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Z;
    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, NumberKey_DoesNotTriggerSelection) {
    EXPECT_CALL(*mockListener, OnPlaySelected()).Times(0);
    EXPECT_CALL(*mockListener, OnQuitSelected()).Times(0);

    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Num1;
    menuScreen->HandleEvent(event);
}

TEST_F(MenuScreenTest, Render_DoesNotCrash) {
    EXPECT_NO_THROW(menuScreen->Render(window));
}

TEST_F(MenuScreenTest, MultipleRenders_DoNotCrash) {
    for (int i = 0; i < 10; ++i) {
        EXPECT_NO_THROW(menuScreen->Render(window));
    }
}

TEST_F(MenuScreenTest, RenderAfterNavigation_DoesNotCrash) {
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::Down;
    menuScreen->HandleEvent(event);

    EXPECT_NO_THROW(menuScreen->Render(window));
}

TEST_F(MenuScreenTest, KeyboardThenMouse_BothWork) {
    menuScreen->Render(window);

    sf::Event keyEvent;
    keyEvent.type = sf::Event::KeyPressed;
    keyEvent.key.code = sf::Keyboard::Down;
    menuScreen->HandleEvent(keyEvent);

    sf::Event mouseEvent;
    mouseEvent.type = sf::Event::MouseMoved;
    mouseEvent.mouseMove.x = 400;
    mouseEvent.mouseMove.y = 250;
    menuScreen->HandleEvent(mouseEvent);

    EXPECT_CALL(*mockListener, OnPlaySelected()).Times(1);
    sf::Event clickEvent;
    clickEvent.type = sf::Event::MouseButtonPressed;
    clickEvent.mouseButton.button = sf::Mouse::Left;
    clickEvent.mouseButton.x = 400;
    clickEvent.mouseButton.y = 250;
    menuScreen->HandleEvent(clickEvent);
}

TEST_F(MenuScreenTest, MouseThenKeyboard_BothWork) {
    menuScreen->Render(window);

    sf::Event mouseEvent;
    mouseEvent.type = sf::Event::MouseMoved;
    mouseEvent.mouseMove.x = 400;
    mouseEvent.mouseMove.y = 350;
    menuScreen->HandleEvent(mouseEvent);

    sf::Event keyEvent;
    keyEvent.type = sf::Event::KeyPressed;
    keyEvent.key.code = sf::Keyboard::Up;
    menuScreen->HandleEvent(keyEvent);

    EXPECT_CALL(*mockListener, OnPlaySelected()).Times(1);
    sf::Event enterEvent;
    enterEvent.type = sf::Event::KeyPressed;
    enterEvent.key.code = sf::Keyboard::Enter;
    menuScreen->HandleEvent(enterEvent);
}
