#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Application.hpp"

using namespace Pacman;

class ApplicationTest : public ::testing::Test {
protected:
    void SetUp() override {
        app = std::make_unique<Application>();
    }

    std::unique_ptr<Application> app;
};

TEST_F(ApplicationTest, InitialState_ShouldNotStartGameOrQuit) {
    EXPECT_FALSE(app->ShouldStartGame());
    EXPECT_FALSE(app->ShouldQuit());
}

TEST_F(ApplicationTest, OnPlaySelected_ShouldSetStartGameFlag) {
    app->OnPlaySelected();
    EXPECT_TRUE(app->ShouldStartGame());
    EXPECT_FALSE(app->ShouldQuit());
}

TEST_F(ApplicationTest, OnQuitSelected_ShouldSetQuitFlag) {
    app->OnQuitSelected();
    EXPECT_FALSE(app->ShouldStartGame());
    EXPECT_TRUE(app->ShouldQuit());
}

TEST_F(ApplicationTest, MultiplePlaySelections_ShouldKeepStartGameFlagSet) {
    app->OnPlaySelected();
    app->OnPlaySelected();
    EXPECT_TRUE(app->ShouldStartGame());
    EXPECT_FALSE(app->ShouldQuit());
}

TEST_F(ApplicationTest, MultipleQuitSelections_ShouldKeepQuitFlagSet) {
    app->OnQuitSelected();
    app->OnQuitSelected();
    EXPECT_FALSE(app->ShouldStartGame());
    EXPECT_TRUE(app->ShouldQuit());
}

TEST_F(ApplicationTest, PlayThenQuit_ShouldHaveBothFlagsSet) {
    app->OnPlaySelected();
    app->OnQuitSelected();
    EXPECT_TRUE(app->ShouldStartGame());
    EXPECT_TRUE(app->ShouldQuit());
}

TEST_F(ApplicationTest, QuitThenPlay_ShouldHaveBothFlagsSet) {
    app->OnQuitSelected();
    app->OnPlaySelected();
    EXPECT_TRUE(app->ShouldStartGame());
    EXPECT_TRUE(app->ShouldQuit());
}

TEST_F(ApplicationTest, ImplementsIMenuListener) {
    IMenuListener* listener = app.get();
    EXPECT_NE(listener, nullptr);
}

TEST_F(ApplicationTest, ShouldMethodsAreConst) {
    const Application& constApp = *app;
    EXPECT_FALSE(constApp.ShouldStartGame());
    EXPECT_FALSE(constApp.ShouldQuit());
}

TEST_F(ApplicationTest, FlagsAreIndependent) {
    app->OnPlaySelected();
    EXPECT_TRUE(app->ShouldStartGame());
    EXPECT_FALSE(app->ShouldQuit());

    app->OnQuitSelected();
    EXPECT_TRUE(app->ShouldStartGame());
    EXPECT_TRUE(app->ShouldQuit());
}
