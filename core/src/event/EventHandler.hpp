#pragma once

#include "IBaseEventHandler.hpp"
#include <functional>
#include <typeindex>

namespace Cosmic {

    template<typename T>
    class EventHandler : public IBaseEventHandler {
    public:
        static_assert(std::is_base_of_v<IEvent, T>, "T must be derived from Event");

        using HandlerFunction = std::function<void(T&)>;

        EventHandler(HandlerFunction handler) : handlerFunction(std::move(handler)) {}

        void handle(IEvent& event) override {
            if (event.getType() == std::type_index(typeid(T))) {
                T& typedEvent = static_cast<T&>(event);
                handlerFunction(typedEvent);
            }
        }

        std::type_index getEventType() const override {
            return std::type_index(typeid(T));
        }

    private:
        HandlerFunction handlerFunction;
    };

}
