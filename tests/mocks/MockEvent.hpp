#pragma once

#include <gmock/gmock.h>
#include <string>
#include <typeindex>
#include <Cosmic.hpp>

class MockEvent : public Cosmic::IEvent {
public:
    MOCK_METHOD(std::string, getName, (), (const, override));
    MOCK_METHOD(std::type_index, getType, (), (const, override));
};
