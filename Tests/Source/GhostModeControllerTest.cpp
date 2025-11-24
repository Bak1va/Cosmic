#include <gtest/gtest.h>
#include "GhostModeController.hpp"

using namespace Pacman;

class GhostModeControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        controller = std::make_unique<GhostModeController>();
    }

    std::unique_ptr<GhostModeController> controller;
};

TEST_F(GhostModeControllerTest, InitialState_IsScatterMode) {
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
}

TEST_F(GhostModeControllerTest, InitialState_NotFrightened) {
    EXPECT_FALSE(controller->IsFrightened());
}

TEST_F(GhostModeControllerTest, InitialState_NoFrightenedTime) {
    EXPECT_FLOAT_EQ(controller->GetFrightenedTimeRemaining(), 0.0f);
}

TEST_F(GhostModeControllerTest, InitialState_NotFrightenedWarning) {
    EXPECT_FALSE(controller->IsFrightenedWarning());
}

TEST_F(GhostModeControllerTest, InitialState_NoModeChange) {
    EXPECT_FALSE(controller->ShouldReverseDirection());
}

TEST_F(GhostModeControllerTest, Reset_RestoresScatterMode) {
    controller->Update(10.0f);
    controller->Reset();
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
}

TEST_F(GhostModeControllerTest, Reset_ClearsFrightened) {
    controller->TriggerFrightenedMode();
    controller->Reset();
    EXPECT_FALSE(controller->IsFrightened());
}

TEST_F(GhostModeControllerTest, Reset_ResetsTimers) {
    controller->Update(5.0f);
    controller->Reset();
    controller->Update(0.1f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
}

TEST_F(GhostModeControllerTest, FirstTransition_ScatterToChase) {
    controller->Update(7.0f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
}

TEST_F(GhostModeControllerTest, SecondTransition_ChaseToScatter) {
    controller->Update(27.0f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
}

TEST_F(GhostModeControllerTest, ThirdTransition_ScatterToChaseAgain) {
    controller->Update(34.0f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
}

TEST_F(GhostModeControllerTest, FourthTransition_ChaseToScatter) {
    controller->Update(54.0f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
}

TEST_F(GhostModeControllerTest, FinalTransition_ToPermanentChase) {
    controller->Update(85.0f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);

    controller->Update(1000.0f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
}

TEST_F(GhostModeControllerTest, IncrementalUpdates_ProduceSameResult) {
    auto controller2 = std::make_unique<GhostModeController>();

    controller->Update(10.0f);

    controller2->Update(5.0f);
    controller2->Update(5.0f);

    EXPECT_EQ(controller->GetCurrentMode(), controller2->GetCurrentMode());
}

TEST_F(GhostModeControllerTest, SmallTimeSteps_WorkCorrectly) {
    for (int i = 0; i < 70; ++i) {
        controller->Update(0.1f);
    }
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
}

TEST_F(GhostModeControllerTest, ShouldReverseDirection_TrueOnFirstTransition) {
    controller->Update(7.0f);
    EXPECT_TRUE(controller->ShouldReverseDirection());
}

TEST_F(GhostModeControllerTest, ShouldReverseDirection_FalseAfterCheck) {
    controller->Update(7.0f);
    controller->ShouldReverseDirection();
    EXPECT_FALSE(controller->ShouldReverseDirection());
}

TEST_F(GhostModeControllerTest, ShouldReverseDirection_ResetsBetweenTransitions) {
    controller->Update(7.0f);
    EXPECT_TRUE(controller->ShouldReverseDirection());
    EXPECT_FALSE(controller->ShouldReverseDirection());

    controller->Update(20.0f);
    EXPECT_TRUE(controller->ShouldReverseDirection());
    EXPECT_FALSE(controller->ShouldReverseDirection());
}

TEST_F(GhostModeControllerTest, ShouldReverseDirection_FalseWithoutTransition) {
    controller->Update(3.0f);
    EXPECT_FALSE(controller->ShouldReverseDirection());
}

TEST_F(GhostModeControllerTest, TriggerFrightenedMode_SetsFrightenedState) {
    controller->TriggerFrightenedMode();
    EXPECT_TRUE(controller->IsFrightened());
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Frightened);
}

TEST_F(GhostModeControllerTest, TriggerFrightenedMode_SetsCorrectDuration) {
    controller->TriggerFrightenedMode();
    EXPECT_FLOAT_EQ(controller->GetFrightenedTimeRemaining(), GameConfig::PowerUpDuration);
}

TEST_F(GhostModeControllerTest, TriggerFrightenedMode_CustomDuration) {
    controller->TriggerFrightenedMode(10.0f);
    EXPECT_FLOAT_EQ(controller->GetFrightenedTimeRemaining(), 10.0f);
}

TEST_F(GhostModeControllerTest, FrightenedMode_CountsDown) {
    controller->TriggerFrightenedMode(6.0f);
    controller->Update(2.0f);
    EXPECT_FLOAT_EQ(controller->GetFrightenedTimeRemaining(), 4.0f);
}

TEST_F(GhostModeControllerTest, FrightenedMode_ExpiresAfterDuration) {
    controller->TriggerFrightenedMode(3.0f);
    controller->Update(3.5f);
    EXPECT_FALSE(controller->IsFrightened());
}

TEST_F(GhostModeControllerTest, FrightenedMode_ReturnsToScatter) {
    controller->TriggerFrightenedMode(3.0f);
    controller->Update(3.5f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
}

TEST_F(GhostModeControllerTest, FrightenedMode_ReturnsToChase) {
    controller->Update(10.0f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);

    controller->TriggerFrightenedMode(2.0f);
    controller->Update(2.5f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
}

TEST_F(GhostModeControllerTest, FrightenedMode_PausesWaveTimer) {
    controller->Update(5.0f);
    controller->TriggerFrightenedMode(10.0f);
    controller->Update(10.5f);

    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
}

TEST_F(GhostModeControllerTest, FrightenedMode_CanRetrigger) {
    controller->TriggerFrightenedMode(2.0f);
    controller->Update(1.0f);

    controller->TriggerFrightenedMode(3.0f);
    EXPECT_FLOAT_EQ(controller->GetFrightenedTimeRemaining(), 3.0f);
}

TEST_F(GhostModeControllerTest, IsFrightenedWarning_FalseWhenNotFrightened) {
    EXPECT_FALSE(controller->IsFrightenedWarning());
}

TEST_F(GhostModeControllerTest, IsFrightenedWarning_FalseAtStart) {
    controller->TriggerFrightenedMode(6.0f);
    EXPECT_FALSE(controller->IsFrightenedWarning());
}

TEST_F(GhostModeControllerTest, IsFrightenedWarning_TrueNearEnd) {
    controller->TriggerFrightenedMode(6.0f);
    controller->Update(4.5f);
    EXPECT_TRUE(controller->IsFrightenedWarning());
}

TEST_F(GhostModeControllerTest, IsFrightenedWarning_TrueAtWarningThreshold) {
    controller->TriggerFrightenedMode(6.0f);
    controller->Update(4.0f);
    EXPECT_TRUE(controller->IsFrightenedWarning());
}

TEST_F(GhostModeControllerTest, IsFrightenedWarning_FalseJustAboveThreshold) {
    controller->TriggerFrightenedMode(6.0f);
    controller->Update(3.9f);
    EXPECT_FALSE(controller->IsFrightenedWarning());
}

TEST_F(GhostModeControllerTest, IsFrightenedWarning_FalseAfterExpiry) {
    controller->TriggerFrightenedMode(6.0f);
    controller->Update(7.0f);
    EXPECT_FALSE(controller->IsFrightenedWarning());
}

TEST_F(GhostModeControllerTest, MultipleFrightenedModes_OverwriteTimer) {
    controller->TriggerFrightenedMode(5.0f);
    controller->Update(2.0f);
    EXPECT_FLOAT_EQ(controller->GetFrightenedTimeRemaining(), 3.0f);

    controller->TriggerFrightenedMode(6.0f);
    EXPECT_FLOAT_EQ(controller->GetFrightenedTimeRemaining(), 6.0f);
}

TEST_F(GhostModeControllerTest, FrightenedDuringPermanentChase_ReturnsToChase) {
    controller->Update(1000.0f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);

    controller->TriggerFrightenedMode(2.0f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Frightened);

    controller->Update(3.0f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
}

TEST_F(GhostModeControllerTest, ZeroTimeUpdate_NoStateChange) {
    GhostMode initialMode = controller->GetCurrentMode();
    controller->Update(0.0f);
    EXPECT_EQ(controller->GetCurrentMode(), initialMode);
}

TEST_F(GhostModeControllerTest, VerySmallTimeUpdate_WorksCorrectly) {
    controller->Update(0.001f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
}

TEST_F(GhostModeControllerTest, VeryLargeTimeUpdate_HandledCorrectly) {
    controller->Update(100000.0f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
}

TEST_F(GhostModeControllerTest, FrightenedWithZeroTime_ExpiredImmediately) {
    controller->TriggerFrightenedMode(0.0f);
    EXPECT_TRUE(controller->IsFrightened());
    controller->Update(0.1f);
    EXPECT_FALSE(controller->IsFrightened());
}

TEST_F(GhostModeControllerTest, ExactTransitionTime_ModeSwitches) {
    controller->Update(7.0f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
    EXPECT_TRUE(controller->ShouldReverseDirection());
}

TEST_F(GhostModeControllerTest, SlightlyOverTransitionTime_ModeSwitches) {
    controller->Update(7.001f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
}

TEST_F(GhostModeControllerTest, SlightlyUnderTransitionTime_NoModeSwitch) {
    controller->Update(6.999f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
}

TEST_F(GhostModeControllerTest, AllFourWaves_HaveCorrectDurations) {
    controller->Update(6.9f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
    controller->Update(0.2f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);

    controller->Update(19.9f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
    controller->Update(0.2f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);

    controller->Update(6.9f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
    controller->Update(0.2f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);

    controller->Update(19.9f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
    controller->Update(0.2f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
}

TEST_F(GhostModeControllerTest, Wave2_HasShorterScatterDuration) {
    controller->Update(54.0f);

    controller->Update(4.9f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
    controller->Update(0.2f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
}

TEST_F(GhostModeControllerTest, Wave3_HasShorterScatterDuration) {
    controller->Update(79.0f);

    controller->Update(4.9f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Scatter);
    controller->Update(0.2f);
    EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
}

TEST_F(GhostModeControllerTest, PermanentChase_NeverChanges) {
    controller->Update(1000.0f);

    for (int i = 0; i < 100; ++i) {
        controller->Update(1.0f);
        EXPECT_EQ(controller->GetCurrentMode(), GhostMode::Chase);
    }
}
