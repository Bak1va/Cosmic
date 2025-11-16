#pragma once

#include "IEvent.hpp"

namespace Cosmic {

    class GameStartEvent : public IEvent {
    public:
        GameStartEvent() = default;
        EVENT_CLASS_TYPE(GameStartEvent)
    };

    class GameQuitEvent : public IEvent {
    public:
        GameQuitEvent() = default;
        EVENT_CLASS_TYPE(GameQuitEvent)
    };

    class GameUpdateEvent : public IEvent {
    public:
        explicit GameUpdateEvent(float deltaTime) : deltaTime(deltaTime) {}

        float getDeltaTime() const { return deltaTime; }

        EVENT_CLASS_TYPE(GameUpdateEvent)

    private:
        float deltaTime;
    };


}

