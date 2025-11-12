#pragma once

#include "IEvent.hpp"

namespace Cosmic {

    class IEventListener {
    public:
        virtual ~IEventListener() = default;

        virtual void onEvent(IEvent& event) = 0;
    };
}
