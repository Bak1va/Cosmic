#pragma once

#include "Cosmic.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <memory>

namespace Cosmic {
    class App {
    public:
        App(unsigned int width = 800u, unsigned int height = 600u, const std::string& title = "Orbyte");
        void run();

    private:
        void processEvents();
        void update(const sf::Time& dt);
        void render();

        sf::RenderWindow window;
        uint32_t width;
        uint32_t height;
        std::string title;
        sf::Clock clock;

        std::unique_ptr<logic::IGame> game;
    };
}
