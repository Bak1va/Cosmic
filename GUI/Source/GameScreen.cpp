#include "GameScreen.hpp"
#include "GameConfig.hpp"
#include <iostream>
#include <functional>

namespace Pacman {

    GameScreen::GameScreen() = default;

    bool GameScreen::LoadAssets(const std::string& assetPath) {
        bool success = true;
        
        if(!pacmanTexture_.loadFromFile(assetPath + "/Pacman16.png")) {
            std::cerr << "Failed to load Pacman texture\n";
            success = false;
        }
        
        if(!ghostTexture_.loadFromFile(assetPath + "/Ghost16.png")) {
            std::cerr << "Failed to load Ghost texture from: " << assetPath << "/Ghost16.png\n";
            std::cerr << "Using fallback colored ghost rendering\n";
            success = false;
        } else {
            std::cout << "Ghost texture loaded successfully: "
                      << ghostTexture_.getSize().x << "x" << ghostTexture_.getSize().y << " pixels\n";
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

    void GameScreen::SetGameEngine(std::shared_ptr<IGameEngine> gameEngine) {
        gameEngine_ = std::move(gameEngine);
    }

    void GameScreen::OnTileUpdated(const TileUpdate& update) {
        updatedTiles_.push_back(update);
    }

    void GameScreen::OnPlayerStateChanged(const PlayerState& state) {
        playerState_ = state;
    }

    void GameScreen::OnGameStateChanged(GameState state) {
        gameState_ = state;
    }

    void GameScreen::OnGhostsUpdated(const std::vector<GhostState>& ghosts) {
        ghostStates_ = ghosts;
    }

    void GameScreen::SetPlayCallback(std::function<void()> cb) {
        playCallback_ = std::move(cb);
    }

    void GameScreen::HandleEvent(const sf::Event& event, sf::RenderWindow& window) {
        if (gameState_ != GameState::GameOver && gameState_ != GameState::Victory) return;

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            if (playAgainButtonRect_.contains(worldPos)) {
                if (playCallback_) playCallback_();
            }
        }
    }

    void GameScreen::UpdateAnimations() {
        animationTimer_ += 0.016f; // ~60fps
        
        if(animationTimer_ > 0.15f) {
            animationTimer_ = 0.0f;
            pacmanFrame_ = (pacmanFrame_ + 1) % GameConfig::PacmanFrameCount;
            ghostFrame_ = (ghostFrame_ + 1) % GameConfig::GhostFrameCount;
        }
    }

    void GameScreen::RenderMap(sf::RenderWindow& window) {
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

    void GameScreen::RenderPlayer(sf::RenderWindow& window) {
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

    void GameScreen::RenderGhosts(sf::RenderWindow& window) {
        ghostSprites_.clear();
        
        // TEMPORARY: Force fallback colored rendering to ensure all ghosts are visible
        // We'll use colored circles with proper colors for each ghost type
        bool useTexture = false; // Disabled until sprite sheet layout is corrected

        for(const auto& ghost : ghostStates_) {
            if(useTexture) {
                // Sprite rendering (disabled for now)
                sf::Sprite ghostSprite(ghostTexture_);
                int row = 0;
                int col = 0;

                if(ghost.IsEaten) {
                    row = 2;
                    switch(ghost.CurrentDirection) {
                        case Direction::Right: col = 2; break;
                        case Direction::Left:  col = 3; break;
                        case Direction::Up:    col = 4; break;
                        case Direction::Down:  col = 5; break;
                        default: col = 2; break;
                    }
                } else if(ghost.IsFrightened) {
                    row = 2;
                    col = ghostFrame_;
                } else {
                    switch(ghost.Type) {
                        case GhostType::Red:
                            row = 0;
                            col = 0 + ghostFrame_;
                            break;
                        case GhostType::Pink:
                            row = 0;
                            col = 2 + ghostFrame_;
                            break;
                        case GhostType::Blue:
                            row = 1;
                            col = 0 + ghostFrame_;
                            break;
                        case GhostType::Orange:
                            row = 1;
                            col = 2 + ghostFrame_;
                            break;
                        default:
                            row = 0;
                            col = 0;
                            break;
                    }
                }

                ghostSprite.setTextureRect({
                    col * TILE_SIZE,
                    row * TILE_SIZE,
                    TILE_SIZE,
                    TILE_SIZE
                });
                ghostSprite.setPosition(
                    static_cast<float>(ghost.Position.X * TILE_SIZE),
                    static_cast<float>(ghost.Position.Y * TILE_SIZE)
                );

                window.draw(ghostSprite);
            } else {
                // Fallback: Use colored shapes to create better-looking ghosts
                float posX = static_cast<float>(ghost.Position.X * TILE_SIZE);
                float posY = static_cast<float>(ghost.Position.Y * TILE_SIZE);

                if(ghost.IsEaten) {
                    // Draw only eyes when eaten
                    sf::CircleShape eyeWhite(3.5f);
                    eyeWhite.setFillColor(sf::Color::White);

                    sf::CircleShape pupil(1.5f);
                    pupil.setFillColor(sf::Color::Blue);

                    // Left eye
                    eyeWhite.setPosition(posX + 3, posY + 5);
                    window.draw(eyeWhite);
                    pupil.setPosition(posX + 5, posY + 6);
                    window.draw(pupil);

                    // Right eye
                    eyeWhite.setPosition(posX + 10, posY + 5);
                    window.draw(eyeWhite);
                    pupil.setPosition(posX + 12, posY + 6);
                    window.draw(pupil);
                } else {
                    // Draw ghost body
                    sf::Color ghostColor;
                    if(ghost.IsFrightened) {
                        ghostColor = sf::Color(0, 0, 200); // Dark blue when frightened
                    } else {
                        switch(ghost.Type) {
                            case GhostType::Red:    ghostColor = sf::Color(255, 0, 0); break;     // Red
                            case GhostType::Pink:   ghostColor = sf::Color(255, 184, 222); break; // Pink
                            case GhostType::Blue:   ghostColor = sf::Color(0, 255, 255); break;   // Cyan
                            case GhostType::Orange: ghostColor = sf::Color(255, 165, 0); break;   // Orange
                            default: ghostColor = sf::Color::Red; break;
                        }
                    }

                    // Draw body as rounded rectangle (circle for top, rectangle for body)
                    sf::CircleShape head(8.0f);
                    head.setFillColor(ghostColor);
                    head.setPosition(posX, posY);
                    window.draw(head);

                    sf::RectangleShape body(sf::Vector2f(16.0f, 10.0f));
                    body.setFillColor(ghostColor);
                    body.setPosition(posX, posY + 6);
                    window.draw(body);

                    // Draw eyes
                    sf::CircleShape eyeWhite(3.0f);
                    eyeWhite.setFillColor(sf::Color::White);

                    sf::CircleShape pupil(1.5f);
                    pupil.setFillColor(sf::Color::Black);

                    // Left eye
                    eyeWhite.setPosition(posX + 3, posY + 4);
                    window.draw(eyeWhite);
                    pupil.setPosition(posX + 4.5f, posY + 5.5f);
                    window.draw(pupil);

                    // Right eye
                    eyeWhite.setPosition(posX + 10, posY + 4);
                    window.draw(eyeWhite);
                    pupil.setPosition(posX + 11.5f, posY + 5.5f);
                    window.draw(pupil);
                }
            }
        }
    }

    void GameScreen::RenderHud(sf::RenderWindow& window) {
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
                float centerX = static_cast<float>(window.getSize().x) / 2.0f;
                float centerY = static_cast<float>(window.getSize().y) / 2.0f;
                gameOverText.setPosition(centerX, centerY - 40.0f); // move up to leave space for button
                window.draw(gameOverText);

                const float btnW = 200.0f;
                const float btnH = 48.0f;
                sf::Vector2f btnPos(centerX - btnW / 2.0f, centerY + 10.0f);

                sf::RectangleShape btnShape(sf::Vector2f(btnW, btnH));
                btnShape.setFillColor(sf::Color(60, 60, 60));
                btnShape.setOutlineColor(sf::Color::White);
                btnShape.setOutlineThickness(2.0f);
                btnShape.setPosition(btnPos);
                window.draw(btnShape);

                sf::Text playText;
                playText.setFont(hudFont_);
                playText.setCharacterSize(24);
                playText.setFillColor(sf::Color::White);
                playText.setString("Play Again");
                sf::FloatRect ptBounds = playText.getLocalBounds();
                playText.setOrigin(ptBounds.width / 2.0f, ptBounds.height / 2.0f);
                playText.setPosition(btnPos.x + btnW / 2.0f, btnPos.y + btnH / 2.0f - 4.0f);
                window.draw(playText);

                playAgainButtonRect_ = sf::FloatRect(btnPos.x, btnPos.y, btnW, btnH);

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

                const float btnW = 200.0f;
                const float btnH = 48.0f;
                float centerX = static_cast<float>(window.getSize().x) / 2.0f;
                float centerY = static_cast<float>(window.getSize().y) / 2.0f;
                sf::Vector2f btnPos(centerX - btnW / 2.0f, centerY + 10.0f);

                sf::RectangleShape btnShape(sf::Vector2f(btnW, btnH));
                btnShape.setFillColor(sf::Color(60, 60, 60));
                btnShape.setOutlineColor(sf::Color::White);
                btnShape.setOutlineThickness(2.0f);
                btnShape.setPosition(btnPos);
                window.draw(btnShape);

                sf::Text playText;
                playText.setFont(hudFont_);
                playText.setCharacterSize(24);
                playText.setFillColor(sf::Color::White);
                playText.setString("Play Again");
                sf::FloatRect ptBounds = playText.getLocalBounds();
                playText.setOrigin(ptBounds.width / 2.0f, ptBounds.height / 2.0f);
                playText.setPosition(btnPos.x + btnW / 2.0f, btnPos.y + btnH / 2.0f - 4.0f);
                window.draw(playText);

                playAgainButtonRect_ = sf::FloatRect(btnPos.x, btnPos.y, btnW, btnH);
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

    void GameScreen::Render(sf::RenderWindow& window) {
        UpdateAnimations();
        
        window.clear(sf::Color::Black);
        
        RenderMap(window);
        RenderPlayer(window);
        RenderGhosts(window);
        RenderHud(window);
        
        window.display();
    }

}