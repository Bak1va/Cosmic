#pragma once

#include "IEvent.hpp"
#include <typeindex>

namespace Cosmic {

    class IBaseEventHandler {
    public:
        virtual ~IBaseEventHandler() = default;
        virtual void handle(IEvent& event) = 0;
        virtual std::type_index getEventType() const = 0;
    };
}
