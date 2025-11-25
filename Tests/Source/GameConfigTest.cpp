#include <gtest/gtest.h>
#include "GameConfig.hpp"

using namespace Pacman;

TEST(GameConfigTest, MapWidth_IsPositive) {
    EXPECT_GT(GameConfig::MapWidth, 0);
}

TEST(GameConfigTest, MapHeight_IsPositive) {
    EXPECT_GT(GameConfig::MapHeight, 0);
}

TEST(GameConfigTest, MapWidth_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::MapWidth, 28);
}

TEST(GameConfigTest, MapHeight_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::MapHeight, 31);
}

TEST(GameConfigTest, PelletScore_IsPositive) {
    EXPECT_GT(GameConfig::PelletScore, 0);
}

TEST(GameConfigTest, PowerPelletScore_IsPositive) {
    EXPECT_GT(GameConfig::PowerPelletScore, 0);
}

TEST(GameConfigTest, PowerPelletScore_GreaterThanPelletScore) {
    EXPECT_GT(GameConfig::PowerPelletScore, GameConfig::PelletScore);
}

TEST(GameConfigTest, GhostScore_IsPositive) {
    EXPECT_GT(GameConfig::GhostScore, 0);
}

TEST(GameConfigTest, GhostScoreMultiplier_IsPositive) {
    EXPECT_GT(GameConfig::GhostScoreMultiplier, 0);
}

TEST(GameConfigTest, PelletScore_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::PelletScore, 10);
}

TEST(GameConfigTest, PowerPelletScore_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::PowerPelletScore, 50);
}

TEST(GameConfigTest, GhostScore_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::GhostScore, 50);
}

TEST(GameConfigTest, GhostScoreMultiplier_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::GhostScoreMultiplier, 2);
}

TEST(GameConfigTest, PlayerStepInterval_IsPositive) {
    EXPECT_GT(GameConfig::PlayerStepInterval, 0.0f);
}

TEST(GameConfigTest, GhostStepInterval_IsPositive) {
    EXPECT_GT(GameConfig::GhostStepInterval, 0.0f);
}

TEST(GameConfigTest, GhostFrightenedStepInterval_IsPositive) {
    EXPECT_GT(GameConfig::GhostFrightenedStepInterval, 0.0f);
}

TEST(GameConfigTest, GhostEatenStepInterval_IsPositive) {
    EXPECT_GT(GameConfig::GhostEatenStepInterval, 0.0f);
}

TEST(GameConfigTest, PlayerStepInterval_MatchesExpectedValue) {
    EXPECT_FLOAT_EQ(GameConfig::PlayerStepInterval, 0.12f);
}

TEST(GameConfigTest, GhostStepInterval_MatchesExpectedValue) {
    EXPECT_FLOAT_EQ(GameConfig::GhostStepInterval, 0.16f);
}

TEST(GameConfigTest, GhostFrightenedStepInterval_MatchesExpectedValue) {
    EXPECT_FLOAT_EQ(GameConfig::GhostFrightenedStepInterval, 0.24f);
}

TEST(GameConfigTest, GhostEatenStepInterval_MatchesExpectedValue) {
    EXPECT_FLOAT_EQ(GameConfig::GhostEatenStepInterval, 0.06f);
}

TEST(GameConfigTest, PlayerFasterThanGhost) {
    EXPECT_LT(GameConfig::PlayerStepInterval, GameConfig::GhostStepInterval);
}

TEST(GameConfigTest, FrightenedGhostSlowerThanNormal) {
    EXPECT_GT(GameConfig::GhostFrightenedStepInterval, GameConfig::GhostStepInterval);
}

TEST(GameConfigTest, EatenGhostFasterThanNormal) {
    EXPECT_LT(GameConfig::GhostEatenStepInterval, GameConfig::GhostStepInterval);
}

TEST(GameConfigTest, PowerUpDuration_IsPositive) {
    EXPECT_GT(GameConfig::PowerUpDuration, 0.0f);
}

TEST(GameConfigTest, PowerUpWarningTime_IsPositive) {
    EXPECT_GT(GameConfig::PowerUpWarningTime, 0.0f);
}

TEST(GameConfigTest, PowerUpWarningTime_LessThanDuration) {
    EXPECT_LT(GameConfig::PowerUpWarningTime, GameConfig::PowerUpDuration);
}

TEST(GameConfigTest, PowerUpDuration_MatchesExpectedValue) {
    EXPECT_FLOAT_EQ(GameConfig::PowerUpDuration, 6.0f);
}

TEST(GameConfigTest, PowerUpWarningTime_MatchesExpectedValue) {
    EXPECT_FLOAT_EQ(GameConfig::PowerUpWarningTime, 2.0f);
}

TEST(GameConfigTest, ScatterChaseWaves_IsPositive) {
    EXPECT_GT(GameConfig::ScatterChaseWaves, 0);
}

TEST(GameConfigTest, ScatterChaseWaves_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::ScatterChaseWaves, 4);
}

TEST(GameConfigTest, ScatterDurations_AllPositive) {
    for (int i = 0; i < GameConfig::ScatterChaseWaves; ++i) {
        EXPECT_GT(GameConfig::ScatterDurations[i], 0.0f);
    }
}

TEST(GameConfigTest, ChaseDurations_AllPositive) {
    for (float ChaseDuration : GameConfig::ChaseDurations) {
        EXPECT_GT(ChaseDuration, 0.0f);
    }
}

TEST(GameConfigTest, ScatterDurations_ArraySizeCorrect) {
    EXPECT_EQ(sizeof(GameConfig::ScatterDurations) / sizeof(float),
              static_cast<size_t>(GameConfig::ScatterChaseWaves));
}

TEST(GameConfigTest, ChaseDurations_ArraySizeCorrect) {
    EXPECT_EQ(sizeof(GameConfig::ChaseDurations) / sizeof(float),
              static_cast<size_t>(GameConfig::ScatterChaseWaves));
}

TEST(GameConfigTest, ScatterDurations_MatchExpectedValues) {
    EXPECT_FLOAT_EQ(GameConfig::ScatterDurations[0], 7.0f);
    EXPECT_FLOAT_EQ(GameConfig::ScatterDurations[1], 7.0f);
    EXPECT_FLOAT_EQ(GameConfig::ScatterDurations[2], 5.0f);
    EXPECT_FLOAT_EQ(GameConfig::ScatterDurations[3], 5.0f);
}

TEST(GameConfigTest, ChaseDurations_MatchExpectedValues) {
    EXPECT_FLOAT_EQ(GameConfig::ChaseDurations[0], 20.0f);
    EXPECT_FLOAT_EQ(GameConfig::ChaseDurations[1], 20.0f);
    EXPECT_FLOAT_EQ(GameConfig::ChaseDurations[2], 20.0f);
    EXPECT_FLOAT_EQ(GameConfig::ChaseDurations[3], 99999.0f);
}

TEST(GameConfigTest, LastChaseDuration_IsVeryLarge) {
    EXPECT_GT(GameConfig::ChaseDurations[3], 1000.0f);
}

TEST(GameConfigTest, PinkTargetAhead_IsPositive) {
    EXPECT_GT(GameConfig::PinkTargetAhead, 0);
}

TEST(GameConfigTest, BlueTargetAhead_IsPositive) {
    EXPECT_GT(GameConfig::BlueTargetAhead, 0);
}

TEST(GameConfigTest, OrangeShyDistance_IsPositive) {
    EXPECT_GT(GameConfig::OrangeShyDistance, 0);
}

TEST(GameConfigTest, ElroyDotsThreshold1_IsPositive) {
    EXPECT_GT(GameConfig::ElroyDotsThreshold1, 0);
}

TEST(GameConfigTest, ElroyDotsThreshold2_IsPositive) {
    EXPECT_GT(GameConfig::ElroyDotsThreshold2, 0);
}

TEST(GameConfigTest, ElroyThresholds_Ordered) {
    EXPECT_GT(GameConfig::ElroyDotsThreshold1, GameConfig::ElroyDotsThreshold2);
}

TEST(GameConfigTest, PinkTargetAhead_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::PinkTargetAhead, 4);
}

TEST(GameConfigTest, BlueTargetAhead_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::BlueTargetAhead, 2);
}

TEST(GameConfigTest, OrangeShyDistance_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::OrangeShyDistance, 8);
}

TEST(GameConfigTest, ElroyDotsThreshold1_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::ElroyDotsThreshold1, 20);
}

TEST(GameConfigTest, ElroyDotsThreshold2_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::ElroyDotsThreshold2, 10);
}

TEST(GameConfigTest, RedScatterX_InValidRange) {
    EXPECT_GE(GameConfig::RedScatterX, 0);
    EXPECT_LT(GameConfig::RedScatterX, 30);
}

TEST(GameConfigTest, RedScatterY_CanBeNegative) {
    EXPECT_LT(GameConfig::RedScatterY, GameConfig::MapHeight);
}

TEST(GameConfigTest, PinkScatterX_InValidRange) {
    EXPECT_GE(GameConfig::PinkScatterX, 0);
    EXPECT_LT(GameConfig::PinkScatterX, 30);
}

TEST(GameConfigTest, PinkScatterY_CanBeNegative) {
    EXPECT_LT(GameConfig::PinkScatterY, GameConfig::MapHeight);
}

TEST(GameConfigTest, BlueScatterX_InValidRange) {
    EXPECT_GE(GameConfig::BlueScatterX, 0);
    EXPECT_LT(GameConfig::BlueScatterX, 30);
}

TEST(GameConfigTest, BlueScatterY_CanBeOutOfBounds) {
    EXPECT_GT(GameConfig::BlueScatterY, GameConfig::MapHeight);
}

TEST(GameConfigTest, OrangeScatterX_InValidRange) {
    EXPECT_GE(GameConfig::OrangeScatterX, 0);
    EXPECT_LT(GameConfig::OrangeScatterX, 30);
}

TEST(GameConfigTest, OrangeScatterY_CanBeOutOfBounds) {
    EXPECT_GT(GameConfig::OrangeScatterY, GameConfig::MapHeight);
}

TEST(GameConfigTest, ScatterCorners_AreInDifferentCorners) {
    EXPECT_GT(GameConfig::RedScatterX, 20);
    EXPECT_LT(GameConfig::RedScatterY, 5);

    EXPECT_LT(GameConfig::PinkScatterX, 10);
    EXPECT_LT(GameConfig::PinkScatterY, 5);

    EXPECT_GT(GameConfig::BlueScatterX, 20);
    EXPECT_GT(GameConfig::BlueScatterY, 30);

    EXPECT_LT(GameConfig::OrangeScatterX, 10);
    EXPECT_GT(GameConfig::OrangeScatterY, 30);
}

TEST(GameConfigTest, ScatterCorners_MatchExpectedValues) {
    EXPECT_EQ(GameConfig::RedScatterX, 25);
    EXPECT_EQ(GameConfig::RedScatterY, -3);
    EXPECT_EQ(GameConfig::PinkScatterX, 2);
    EXPECT_EQ(GameConfig::PinkScatterY, -3);
    EXPECT_EQ(GameConfig::BlueScatterX, 27);
    EXPECT_EQ(GameConfig::BlueScatterY, 34);
    EXPECT_EQ(GameConfig::OrangeScatterX, 0);
    EXPECT_EQ(GameConfig::OrangeScatterY, 34);
}

TEST(GameConfigTest, GhostHouseX_InMapBounds) {
    EXPECT_GE(GameConfig::GhostHouseX, 0);
    EXPECT_LT(GameConfig::GhostHouseX, GameConfig::MapWidth);
}

TEST(GameConfigTest, GhostHouseY_InMapBounds) {
    EXPECT_GE(GameConfig::GhostHouseY, 0);
    EXPECT_LT(GameConfig::GhostHouseY, GameConfig::MapHeight);
}

TEST(GameConfigTest, GhostHouseDoorY_InMapBounds) {
    EXPECT_GE(GameConfig::GhostHouseDoorY, 0);
    EXPECT_LT(GameConfig::GhostHouseDoorY, GameConfig::MapHeight);
}

TEST(GameConfigTest, GhostHouseDoor_AboveGhostHouse) {
    EXPECT_LT(GameConfig::GhostHouseDoorY, GameConfig::GhostHouseY);
}

TEST(GameConfigTest, GhostHouse_MatchesExpectedValues) {
    EXPECT_EQ(GameConfig::GhostHouseX, 13);
    EXPECT_EQ(GameConfig::GhostHouseY, 14);
    EXPECT_EQ(GameConfig::GhostHouseDoorY, 12);
}

TEST(GameConfigTest, PlayerStartX_InMapBounds) {
    EXPECT_GE(GameConfig::PlayerStartX, 0);
    EXPECT_LT(GameConfig::PlayerStartX, GameConfig::MapWidth);
}

TEST(GameConfigTest, PlayerStartY_InMapBounds) {
    EXPECT_GE(GameConfig::PlayerStartY, 0);
    EXPECT_LT(GameConfig::PlayerStartY, GameConfig::MapHeight);
}

TEST(GameConfigTest, PlayerStart_MatchesExpectedValues) {
    EXPECT_EQ(GameConfig::PlayerStartX, 13);
    EXPECT_EQ(GameConfig::PlayerStartY, 26);
}

TEST(GameConfigTest, PlayerStart_BelowGhostHouse) {
    EXPECT_GT(GameConfig::PlayerStartY, GameConfig::GhostHouseY);
}

TEST(GameConfigTest, StartingLives_IsPositive) {
    EXPECT_GT(GameConfig::StartingLives, 0);
}

TEST(GameConfigTest, MaxLives_IsPositive) {
    EXPECT_GT(GameConfig::MaxLives, 0);
}

TEST(GameConfigTest, MaxLives_GreaterThanOrEqualStartingLives) {
    EXPECT_GE(GameConfig::MaxLives, GameConfig::StartingLives);
}

TEST(GameConfigTest, ExtraLifeScore_IsPositive) {
    EXPECT_GT(GameConfig::ExtraLifeScore, 0);
}

TEST(GameConfigTest, StartingLives_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::StartingLives, 3);
}

TEST(GameConfigTest, MaxLives_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::MaxLives, 5);
}

TEST(GameConfigTest, ExtraLifeScore_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::ExtraLifeScore, 10000);
}

TEST(GameConfigTest, TileSize_IsPositive) {
    EXPECT_GT(GameConfig::TileSize, 0);
}

TEST(GameConfigTest, PacmanFrameCount_IsPositive) {
    EXPECT_GT(GameConfig::PacmanFrameCount, 0);
}

TEST(GameConfigTest, GhostFrameCount_IsPositive) {
    EXPECT_GT(GameConfig::GhostFrameCount, 0);
}

TEST(GameConfigTest, TileSize_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::TileSize, 16);
}

TEST(GameConfigTest, GhostFrameCount_MatchesExpectedValue) {
    EXPECT_EQ(GameConfig::GhostFrameCount, 2);
}

TEST(GameConfigTest, MapDimensions_CreateReasonableMapSize) {
    int totalTiles = GameConfig::MapWidth * GameConfig::MapHeight;
    EXPECT_GT(totalTiles, 500);
    EXPECT_LT(totalTiles, 2000);
}

TEST(GameConfigTest, ScatterDurations_TotalIsReasonable) {
    float total = 0;
    for (int i = 0; i < GameConfig::ScatterChaseWaves - 1; ++i) {
        total += GameConfig::ScatterDurations[i];
    }
    EXPECT_GT(total, 10.0f);
    EXPECT_LT(total, 60.0f);
}

TEST(GameConfigTest, ChaseDurations_ExcludingLast_AreReasonable) {
    for (int i = 0; i < GameConfig::ScatterChaseWaves - 1; ++i) {
        EXPECT_GT(GameConfig::ChaseDurations[i], 10.0f);
        EXPECT_LT(GameConfig::ChaseDurations[i], 60.0f);
    }
}

TEST(GameConfigTest, GhostHouse_CenteredHorizontally) {
    EXPECT_NEAR(GameConfig::GhostHouseX, GameConfig::MapWidth / 2, 2);
}

TEST(GameConfigTest, PlayerStart_CenteredHorizontally) {
    EXPECT_NEAR(GameConfig::PlayerStartX, GameConfig::MapWidth / 2, 2);
}

TEST(GameConfigTest, StepIntervals_OrderMakesGameplayBalanced) {
    EXPECT_LT(GameConfig::PlayerStepInterval, GameConfig::GhostStepInterval);

    EXPECT_GT(GameConfig::GhostFrightenedStepInterval, GameConfig::PlayerStepInterval);
    EXPECT_GT(GameConfig::GhostFrightenedStepInterval, GameConfig::GhostStepInterval);

    EXPECT_LT(GameConfig::GhostEatenStepInterval, GameConfig::PlayerStepInterval);
    EXPECT_LT(GameConfig::GhostEatenStepInterval, GameConfig::GhostStepInterval);
}

TEST(GameConfigTest, ScoringProgression_MakesSense) {
    EXPECT_LT(GameConfig::PelletScore, GameConfig::PowerPelletScore);
    EXPECT_LT(GameConfig::PelletScore, GameConfig::GhostScore);

    EXPECT_GT(GameConfig::GhostScore * GameConfig::GhostScoreMultiplier,
              GameConfig::GhostScore);
}
