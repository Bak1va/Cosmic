#include "EventDispatcher.hpp"
#include "GameEvents.hpp"
#include <algorithm>

namespace Cosmic {

    EventDispatcher& EventDispatcher::getInstance() {
        static EventDispatcher instance;
        return instance;
    }

    void EventDispatcher::subscribe(std::shared_ptr<IEventListener> listener) {
        listeners.push_back(listener);
    }

    void EventDispatcher::unsubscribe(std::shared_ptr<IEventListener> listener) {
        listeners.erase(
            std::remove_if(listeners.begin(), listeners.end(),
                [&listener](const std::weak_ptr<IEventListener>& weak) {
                    auto shared = weak.lock();
                    return !shared || shared == listener;
                }),
            listeners.end());
    }

    void EventDispatcher::unsubscribe(std::size_t handlerId) {
        auto registryIt = handlerRegistry.find(handlerId);
        if (registryIt != handlerRegistry.end()) {
            std::type_index eventType = registryIt->second;
            auto& handlers = typeHandlers[eventType];

            handlers.erase(
                std::remove_if(handlers.begin(), handlers.end(),
                    [handlerId](const HandlerInfo& info) {
                        return info.first == handlerId;
                    }),
                handlers.end());

            handlerRegistry.erase(registryIt);
        }
    }

    void EventDispatcher::dispatch(IEvent& event) {
        // Notify listeners
        cleanupExpiredListeners();
        for (auto& weakListener : listeners) {
            if (auto listener = weakListener.lock()) {
                listener->onEvent(event);
                if (event.isHandled()) break;
            }
        }

        // Notify typed handlers (only if event not already handled)
        if (!event.isHandled()) {
            std::type_index eventType = event.getType();
            auto it = typeHandlers.find(eventType);
            if (it != typeHandlers.end()) {
                for (auto& handlerInfo : it->second) {
                    handlerInfo.second->handle(event);
                    if (event.isHandled()) break;
                }
            }
        }
    }

    void EventDispatcher::dispatch(std::unique_ptr<IEvent> event) {
        if (event) {
            dispatch(*event);
        }
    }

    void EventDispatcher::clear() {
        listeners.clear();
        typeHandlers.clear();
        handlerRegistry.clear();
        nextHandlerId = 1;
    }

    void EventDispatcher::cleanupExpiredListeners() {
        listeners.erase(
            std::remove_if(listeners.begin(), listeners.end(),
                [](const std::weak_ptr<IEventListener>& weak) {
                    return weak.expired();
                }),
            listeners.end());
    }

}
