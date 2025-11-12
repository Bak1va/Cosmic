#pragma once

#include <string>
#include <typeinfo>
#include <typeindex>

namespace Cosmic {

    class IEvent {
    public:
        virtual ~IEvent() = default;

        virtual std::string getName() const = 0;
        virtual std::type_index getType() const = 0;

        bool isHandled() const { return handled; }
        void setHandled() { handled = true; }

    private:
        bool handled = false;
    };

    #define EVENT_CLASS_TYPE(type) \
        static std::type_index getStaticType() { return std::type_index(typeid(type)); } \
        std::type_index getType() const override { return getStaticType(); } \
        std::string getName() const override { return #type; }
}
