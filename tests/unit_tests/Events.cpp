#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <vector>
#include <string>

#include <Cosmic.hpp>

#include "../mocks/MockEvent.hpp"

using namespace Cosmic;
using ::testing::Return;
using ::testing::NiceMock;


class RecordingListener : public IEventListener {
public:
    RecordingListener(bool markHandled = false) : markHandled(markHandled) {}

    void onEvent(IEvent& event) override {
        received.push_back(event.getName());
        if (markHandled) {
            event.setHandled();
        }
    }

    std::vector<std::string> received;
    bool markHandled = false;
};

struct EventDispatcherTest : public ::testing::Test {
    void SetUp() override {
        EventDispatcher::getInstance().clear();
    }

    void TearDown() override {
        EventDispatcher::getInstance().clear();
    }
};

TEST_F(EventDispatcherTest, MockEventDeliveredToListener) {
    auto& dispatcher = EventDispatcher::getInstance();
    auto listener = std::make_shared<RecordingListener>();
    dispatcher.subscribe(listener);

    NiceMock<MockEvent> mock;
    EXPECT_CALL(mock, getName())
        .WillOnce(Return(std::string("MockEvent")));
    EXPECT_CALL(mock, getType())
        .WillRepeatedly(Return(std::type_index(typeid(MockEvent))));

    dispatcher.dispatch(mock);

    ASSERT_EQ(listener->received.size(), 1u);
    EXPECT_EQ(listener->received[0], "MockEvent");
}

TEST_F(EventDispatcherTest, ListenerReceivesEvent) {
    auto& dispatcher = EventDispatcher::getInstance();
    auto listener = std::make_shared<RecordingListener>();
    dispatcher.subscribe(listener);

    NiceMock<MockEvent> mock;
    EXPECT_CALL(mock, getName())
        .WillOnce(Return(std::string("MockEvent")));
    EXPECT_CALL(mock, getType())
        .WillRepeatedly(Return(std::type_index(typeid(MockEvent))));

    dispatcher.dispatch(mock);

    ASSERT_EQ(listener->received.size(), 1u);
    EXPECT_EQ(listener->received[0], "MockEvent");
}

TEST_F(EventDispatcherTest, TypedHandlerReceivesMatchingEvent) {
    auto& dispatcher = EventDispatcher::getInstance();

    int callCount = 0;
    std::size_t handlerId = dispatcher.subscribe<GameUpdateEvent>([&](GameUpdateEvent& e) {
        ++callCount;
        EXPECT_FLOAT_EQ(e.getDeltaTime(), 0.016f);
    });

    EXPECT_GT(handlerId, 0u);

    GameUpdateEvent ev(0.016f);
    dispatcher.dispatch(ev);
    EXPECT_EQ(callCount, 1);

    dispatcher.unsubscribe(handlerId);
}

TEST_F(EventDispatcherTest, UnsubscribeHandlerById) {
    auto& dispatcher = EventDispatcher::getInstance();

    int callCount = 0;
    std::size_t handlerId = dispatcher.subscribe<GameUpdateEvent>([&](GameUpdateEvent& e) {
        ++callCount;
    });

    dispatcher.unsubscribe(handlerId);

    GameUpdateEvent ev(0.033f);
    dispatcher.dispatch(ev);
    EXPECT_EQ(callCount, 0);
}

TEST_F(EventDispatcherTest, UnsubscribeListenerBySharedPtr) {
    auto& dispatcher = EventDispatcher::getInstance();
    auto listener = std::make_shared<RecordingListener>();
    dispatcher.subscribe(listener);

    {
        NiceMock<MockEvent> mock1;
        EXPECT_CALL(mock1, getName())
            .WillOnce(Return(std::string("MockEvent")));
        EXPECT_CALL(mock1, getType())
            .WillRepeatedly(Return(std::type_index(typeid(MockEvent))));
        dispatcher.dispatch(mock1);
    }
    EXPECT_EQ(listener->received.size(), 1u);

    dispatcher.unsubscribe(listener);

    {
        NiceMock<MockEvent> mock2;
        EXPECT_CALL(mock2, getType())
            .WillOnce(Return(std::type_index(typeid(MockEvent))));
        dispatcher.dispatch(mock2);
    }

    EXPECT_EQ(listener->received.size(), 1u);
}

TEST_F(EventDispatcherTest, DispatchUniquePtr) {
    auto& dispatcher = EventDispatcher::getInstance();
    auto listener = std::make_shared<RecordingListener>();
    dispatcher.subscribe(listener);

    auto mockPtr = std::make_unique<NiceMock<MockEvent>>();
    EXPECT_CALL(*mockPtr, getName())
        .WillOnce(Return(std::string("MockEvent")));
    EXPECT_CALL(*mockPtr, getType())
        .WillRepeatedly(Return(std::type_index(typeid(MockEvent))));

    dispatcher.dispatch(std::move(mockPtr));

    ASSERT_EQ(listener->received.size(), 1u);
    EXPECT_EQ(listener->received[0], "MockEvent");
}

TEST_F(EventDispatcherTest, HandledStopsPropagation) {
    auto& dispatcher = EventDispatcher::getInstance();

    auto listenerA = std::make_shared<RecordingListener>(true);
    dispatcher.subscribe(listenerA);

    auto listenerB = std::make_shared<RecordingListener>();
    dispatcher.subscribe(listenerB);

    int typedHandlerCalls = 0;
    std::size_t handlerId = dispatcher.subscribe<GameStartEvent>([&](GameStartEvent& e) {
        ++typedHandlerCalls;
    });

    NiceMock<MockEvent> mock;
    EXPECT_CALL(mock, getName())
        .WillRepeatedly(Return(std::string("MockEvent")));
    EXPECT_CALL(mock, getType())
        .WillRepeatedly(Return(std::type_index(typeid(GameStartEvent))));

    dispatcher.dispatch(mock);

    EXPECT_EQ(listenerA->received.size(), 1u);
    EXPECT_EQ(listenerB->received.size(), 0u);

    EXPECT_EQ(typedHandlerCalls, 0);

    dispatcher.unsubscribe(handlerId);
}

TEST_F(EventDispatcherTest, ExpiredListenersAreCleanedUp) {
    auto& dispatcher = EventDispatcher::getInstance();

    {
        auto tempListener = std::make_shared<RecordingListener>(true);
        dispatcher.subscribe(tempListener);
    }

    int typedCalls = 0;
    std::size_t handlerId = dispatcher.subscribe<GameStartEvent>([&](GameStartEvent& e) {
        ++typedCalls;
    });

    NiceMock<MockEvent> mock;
    EXPECT_CALL(mock, getName())
        .WillRepeatedly(Return(std::string("MockEvent")));
    EXPECT_CALL(mock, getType())
        .WillRepeatedly(Return(std::type_index(typeid(GameStartEvent))));

    dispatcher.dispatch(mock);
    EXPECT_EQ(typedCalls, 1);

    dispatcher.unsubscribe(handlerId);
}

TEST_F(EventDispatcherTest, ClearResetsState) {
    auto& dispatcher = EventDispatcher::getInstance();

    auto listener = std::make_shared<RecordingListener>();
    dispatcher.subscribe(listener);

    int typedCalls = 0;
    dispatcher.subscribe<GameQuitEvent>([&](GameQuitEvent& e) {
        ++typedCalls;
    });

    {
        NiceMock<MockEvent> mock;
        EXPECT_CALL(mock, getName())
            .WillOnce(Return(std::string("MockEvent")));
        EXPECT_CALL(mock, getType())
            .WillRepeatedly(Return(std::type_index(typeid(GameQuitEvent))));
        dispatcher.dispatch(mock);
    }

    EXPECT_EQ(listener->received.size(), 1u);
    EXPECT_EQ(typedCalls, 1);

    dispatcher.clear();

    {
        NiceMock<MockEvent> mock2;
        EXPECT_CALL(mock2, getType())
            .WillOnce(Return(std::type_index(typeid(GameQuitEvent))));
        dispatcher.dispatch(mock2);
    }

    EXPECT_EQ(listener->received.size(), 1u);
    EXPECT_EQ(typedCalls, 1);
}
