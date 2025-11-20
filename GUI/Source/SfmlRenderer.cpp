#include "SfmlRenderer.hpp"
#include "GameConfig.hpp"
#include <iostream>

namespace Pacman {

    SfmlRenderer::SfmlRenderer() = default;

    bool SfmlRenderer::LoadAssets(const std::string& assetPath) {
        bool success = true;
        
        if(!pacmanTexture_.loadFromFile(assetPath + "/Pacman16.png")) {
            std::cerr << "Failed to load Pacman texture\n";
            success = false;
        }
        
        if(!ghostTexture_.loadFromFile(assetPath + "/Ghost16.png")) {
            std::cerr << "Failed to load Ghost texture\n";
            success = false;
        }
        
        // Optional textures
        if(!pacmanDeathTexture_.loadFromFile(assetPath + "/PacmanDeath16.png")) {
            std::cerr << "Warning: Failed to load PacmanDeath texture\n";
        }
        
        if(!mapTexture_.loadFromFile(assetPath + "/Map16.png")) {
            std::cerr << "Warning: Failed to load Map texture\n";
        }
        
        // Try to load HUD font
        if(!hudFont_.loadFromFile("C:/Windows/Fonts/arial.ttf") &&
           !hudFont_.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            std::cerr << "Warning: Failed to load HUD font\n";
        }
        
        pacmanSprite_.setTexture(pacmanTexture_);
        
        return success;
    }

    void SfmlRenderer::SetGameEngine(std::shared_ptr<IGameEngine> gameEngine) {
        gameEngine_ = std::move(gameEngine);
    }

    void SfmlRenderer::OnTileUpdated(const TileUpdate& update) {
        updatedTiles_.push_back(update);
    }

    void SfmlRenderer::OnPlayerStateChanged(const PlayerState& state) {
        playerState_ = state;
    }

    void SfmlRenderer::OnGameStateChanged(GameState state) {
        gameState_ = state;
    }

    void SfmlRenderer::OnGhostsUpdated(const std::vector<GhostState>& ghosts) {
        ghostStates_ = ghosts;
    }

    void SfmlRenderer::UpdateAnimations() {
        animationTimer_ += 0.016f; // ~60fps
        
        if(animationTimer_ > 0.15f) {
            animationTimer_ = 0.0f;
            pacmanFrame_ = (pacmanFrame_ + 1) % GameConfig::PacmanFrameCount;
            ghostFrame_ = (ghostFrame_ + 1) % GameConfig::GhostFrameCount;
        }
    }

    void SfmlRenderer::RenderMap(sf::RenderWindow& window) {
        if(!gameEngine_) return;
        
        Vector2 mapSize = gameEngine_->GetMapSize();
        sf::RectangleShape tileShape({
            static_cast<float>(TILE_SIZE), 
            static_cast<float>(TILE_SIZE)
        });
        sf::CircleShape pelletShape;
        
        for(int y = 0; y < mapSize.Y; ++y) {
            for(int x = 0; x < mapSize.X; ++x) {
                TileType tile = gameEngine_->GetTileAt({x, y});
                
                // Draw tile background
                if(tile == TileType::Wall) {
                    tileShape.setFillColor(sf::Color(33, 33, 222));
                } else {
                    tileShape.setFillColor(sf::Color::Black);
                }
                
                tileShape.setPosition(
                    static_cast<float>(x * TILE_SIZE), 
                    static_cast<float>(y * TILE_SIZE)
                );
                window.draw(tileShape);
                
                // Draw pellets
                if(tile == TileType::Pellet) {
                    pelletShape.setRadius(2.0f);
                    pelletShape.setFillColor(sf::Color(255, 184, 174));
                    pelletShape.setOrigin(2.0f, 2.0f);
                    pelletShape.setPosition(
                        static_cast<float>(x * TILE_SIZE) + TILE_SIZE / 2.0f,
                        static_cast<float>(y * TILE_SIZE) + TILE_SIZE / 2.0f
                    );
                    window.draw(pelletShape);
                }
                else if(tile == TileType::PowerPellet) {
                    pelletShape.setRadius(5.0f);
                    pelletShape.setFillColor(sf::Color(255, 184, 174));
                    pelletShape.setOrigin(5.0f, 5.0f);
                    pelletShape.setPosition(
                        static_cast<float>(x * TILE_SIZE) + TILE_SIZE / 2.0f,
                        static_cast<float>(y * TILE_SIZE) + TILE_SIZE / 2.0f
                    );
                    window.draw(pelletShape);
                }
                else if(tile == TileType::GhostDoor) {
                    tileShape.setFillColor(sf::Color(255, 184, 222));
                    tileShape.setPosition(
                        static_cast<float>(x * TILE_SIZE), 
                        static_cast<float>(y * TILE_SIZE)
                    );
                    window.draw(tileShape);
                }
            }
        }
    }

    void SfmlRenderer::RenderPlayer(sf::RenderWindow& window) {
        if(!gameEngine_) return;
        
        // Select animation frame row based on direction
        int row = 0;
        switch(playerState_.CurrentDirection) {
            case Direction::Right: row = 0; break;
            case Direction::Left:  row = 1; break;
            case Direction::Up:    row = 2; break;
            case Direction::Down:  row = 3; break;
            default: row = 0; break;
        }
        
        pacmanSprite_.setTexture(pacmanTexture_);
        pacmanSprite_.setTextureRect({
            pacmanFrame_ * TILE_SIZE, 
            row * TILE_SIZE, 
            TILE_SIZE, 
            TILE_SIZE
        });
        pacmanSprite_.setPosition(
            static_cast<float>(playerState_.Position.X * TILE_SIZE),
            static_cast<float>(playerState_.Position.Y * TILE_SIZE)
        );
        
        window.draw(pacmanSprite_);
    }

    void SfmlRenderer::RenderGhosts(sf::RenderWindow& window) {
        ghostSprites_.clear();
        
        for(const auto& ghost : ghostStates_) {
            sf::Sprite ghostSprite(ghostTexture_);
            
            // Select frame based on direction
            int row = 0;
            switch(ghost.CurrentDirection) {
                case Direction::Right: row = 0; break;
                case Direction::Left:  row = 1; break;
                case Direction::Up:    row = 2; break;
                case Direction::Down:  row = 3; break;
                default: row = 0; break;
            }
            
            // Use different row for frightened state
            if(ghost.IsFrightened) {
                row = 4;
            }
            
            ghostSprite.setTextureRect({
                ghostFrame_ * TILE_SIZE, 
                row * TILE_SIZE, 
                TILE_SIZE, 
                TILE_SIZE
            });
            ghostSprite.setPosition(
                static_cast<float>(ghost.Position.X * TILE_SIZE),
                static_cast<float>(ghost.Position.Y * TILE_SIZE)
            );
            
            ghostSprites_.push_back(ghostSprite);
        }
        
        for(const auto& sprite : ghostSprites_) {
            window.draw(sprite);
        }
    }

    void SfmlRenderer::RenderHud(sf::RenderWindow& window) {
        // Skip if font didn't load
        if(!hudFont_.getInfo().family.empty()) {
            sf::Text scoreText;
            scoreText.setFont(hudFont_);
            scoreText.setCharacterSize(20);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setString(
                "Score: " + std::to_string(playerState_.Score) + 
                "  Lives: " + std::to_string(playerState_.Lives)
            );
            scoreText.setPosition(10.0f, 5.0f);
            window.draw(scoreText);
            
            // Game state overlays
            if(gameState_ == GameState::Paused) {
                sf::Text pauseText;
                pauseText.setFont(hudFont_);
                pauseText.setCharacterSize(32);
                pauseText.setFillColor(sf::Color::Yellow);
                pauseText.setString("PAUSED");
                
                sf::FloatRect bounds = pauseText.getLocalBounds();
                pauseText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
                pauseText.setPosition(
                    static_cast<float>(window.getSize().x) / 2.0f,
                    static_cast<float>(window.getSize().y) / 2.0f
                );
                window.draw(pauseText);
            }
            else if(gameState_ == GameState::GameOver) {
                sf::Text gameOverText;
                gameOverText.setFont(hudFont_);
                gameOverText.setCharacterSize(48);
                gameOverText.setFillColor(sf::Color::Red);
                gameOverText.setString("GAME OVER");
                
                sf::FloatRect bounds = gameOverText.getLocalBounds();
                gameOverText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
                gameOverText.setPosition(
                    static_cast<float>(window.getSize().x) / 2.0f,
                    static_cast<float>(window.getSize().y) / 2.0f
                );
                window.draw(gameOverText);
            }
            else if(gameState_ == GameState::Victory) {
                sf::Text victoryText;
                victoryText.setFont(hudFont_);
                victoryText.setCharacterSize(48);
                victoryText.setFillColor(sf::Color::Green);
                victoryText.setString("YOU WIN!");
                
                sf::FloatRect bounds = victoryText.getLocalBounds();
                victoryText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
                victoryText.setPosition(
                    static_cast<float>(window.getSize().x) / 2.0f,
                    static_cast<float>(window.getSize().y) / 2.0f
                );
                window.draw(victoryText);
            }
            
            // Power-up indicator
            if(playerState_.IsPoweredUp) {
                sf::Text powerText;
                powerText.setFont(hudFont_);
                powerText.setCharacterSize(18);
                powerText.setFillColor(sf::Color::Cyan);
                powerText.setString("POWER UP!");
                powerText.setPosition(10.0f, 35.0f);
                window.draw(powerText);
            }
        }
    }

    void SfmlRenderer::Render(sf::RenderWindow& window) {
        UpdateAnimations();
        
        window.clear(sf::Color::Black);
        
        RenderMap(window);
        RenderPlayer(window);
        RenderGhosts(window);
        RenderHud(window);
        
        window.display();
    }

}