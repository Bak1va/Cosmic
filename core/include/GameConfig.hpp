#pragma once

namespace Cosmic {
    struct GameConfig {
        // Map dimensions
        static constexpr int MapWidth = 28;
        static constexpr int MapHeight = 31;

        // Scoring
        static constexpr int PelletScore = 10;
        static constexpr int PowerPelletScore = 50;
        static constexpr int GhostScore = 200;

        // Timing (in seconds)
        static constexpr float PlayerStepInterval = 0.12f; // ~8.3 tiles/second
        static constexpr float GhostStepInterval = 0.16f; // ~6.25 tiles/second
        static constexpr float PowerUpDuration = 8.0f; // Power-up lasts 8 seconds

        // Gameplay
        static constexpr int StartingLives = 3;
        static constexpr int MaxLives = 5;

        // Animation
        static constexpr int TileSize = 16; // Pixels per tile
        static constexpr int PacmanFrameCount = 4;
        static constexpr int GhostFrameCount = 2;
    };
} // namespace Pacman
