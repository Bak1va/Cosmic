#pragma once

#include "IEvent.hpp"
#include "IEventListener.hpp"
#include "IBaseEventHandler.hpp"
#include "EventHandler.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <algorithm>

namespace Cosmic {

    class EventDispatcher {
    public:

        EventDispatcher(const EventDispatcher&) = delete;
        EventDispatcher& operator=(const EventDispatcher&) = delete;
        EventDispatcher(EventDispatcher&&) = delete;
        EventDispatcher& operator=(EventDispatcher&&) = delete;

        static EventDispatcher& getInstance();

        void subscribe(std::shared_ptr<IEventListener> listener);
        void unsubscribe(std::shared_ptr<IEventListener> listener);
        template<typename T>
        std::size_t subscribe(std::function<void(T&)> handler);
        void unsubscribe(std::size_t handlerId);
        void dispatch(IEvent& event);

        /**
         * Dispatch an event using a unique pointer.
         * @param event Unique pointer to the event to dispatch
         */
        void dispatch(std::unique_ptr<IEvent> event);
        void clear();

        template<typename T>
        std::size_t getHandlerCount() const;

    private:
        EventDispatcher() = default;
        ~EventDispatcher() = default;

        std::vector<std::weak_ptr<IEventListener>> listeners;

        using HandlerInfo = std::pair<std::size_t, std::shared_ptr<IBaseEventHandler>>;
        std::unordered_map<std::type_index, std::vector<HandlerInfo>> typeHandlers;

        std::unordered_map<std::size_t, std::type_index> handlerRegistry;

        std::size_t nextHandlerId = 1;

        void cleanupExpiredListeners();
    };

    template<typename T>
    std::size_t EventDispatcher::subscribe(std::function<void(T&)> handler) {
        std::size_t handlerId = nextHandlerId++;
        std::type_index eventType = std::type_index(typeid(T));

        auto eventHandler = std::make_shared<EventHandler<T>>(std::move(handler));
        typeHandlers[eventType].emplace_back(handlerId, eventHandler);
        handlerRegistry.emplace(handlerId, eventType);

        return handlerId;
    }

    template<typename T>
    std::size_t EventDispatcher::getHandlerCount() const {
        std::type_index eventType = std::type_index(typeid(T));
        auto it = typeHandlers.find(eventType);
        if (it != typeHandlers.end()) {
            return it->second.size();
        }
        return 0;
    }
}
