#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "IGhost.hpp"

using namespace Pacman;

class MockGhost : public IGhost {
public:
    MOCK_METHOD(Vector2, CalculateChaseTarget,
                (const GhostState& ghost, const PlayerState& player, const Vector2& blinkyPosition),
                (const, override));
    MOCK_METHOD(Vector2, GetScatterTarget, (), (const, override));
    MOCK_METHOD(GhostType, GetGhostType, (), (const, override));
};

class IGhostTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockGhost = std::make_unique<MockGhost>();

        ghostState.Position = Vector2{12, 14};
        ghostState.CurrentDirection = Direction::Right;
        ghostState.Type = GhostType::Red;
        ghostState.Mode = GhostMode::Chase;

        playerState.Position = Vector2{13, 26};
        playerState.CurrentDirection = Direction::Up;
        playerState.Score = 0;

        blinkyPosition = Vector2{10, 10};
    }

    std::unique_ptr<MockGhost> mockGhost;
    GhostState ghostState;
    PlayerState playerState;
    Vector2 blinkyPosition;
};

TEST_F(IGhostTest, CalculateChaseTarget_CanBeCalled) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(Vector2{15, 20}));

    Vector2 target = mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
    EXPECT_EQ(target.X, 15);
    EXPECT_EQ(target.Y, 20);
}

TEST_F(IGhostTest, GetScatterTarget_CanBeCalled) {
    EXPECT_CALL(*mockGhost, GetScatterTarget())
        .WillOnce(testing::Return(Vector2{25, -3}));

    Vector2 target = mockGhost->GetScatterTarget();
    EXPECT_EQ(target.X, 25);
    EXPECT_EQ(target.Y, -3);
}

TEST_F(IGhostTest, GetGhostType_CanBeCalled) {
    EXPECT_CALL(*mockGhost, GetGhostType())
        .WillOnce(testing::Return(GhostType::Red));

    GhostType type = mockGhost->GetGhostType();
    EXPECT_EQ(type, GhostType::Red);
}

TEST_F(IGhostTest, CalculateChaseTarget_MultipleCalls) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .Times(3)
        .WillRepeatedly(testing::Return(Vector2{10, 10}));

    for (int i = 0; i < 3; ++i) {
        mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
    }
}

TEST_F(IGhostTest, GetScatterTarget_MultipleCalls_ReturnsSameValue) {
    EXPECT_CALL(*mockGhost, GetScatterTarget())
        .Times(5)
        .WillRepeatedly(testing::Return(Vector2{25, -3}));

    Vector2 target1 = mockGhost->GetScatterTarget();
    Vector2 target2 = mockGhost->GetScatterTarget();

    EXPECT_EQ(target1, target2);
}

TEST_F(IGhostTest, CalculateChaseTarget_DifferentGhostPositions) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .Times(3)
        .WillRepeatedly(testing::Return(Vector2{0, 0}));

    ghostState.Position = Vector2{0, 0};
    mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);

    ghostState.Position = Vector2{27, 30};
    mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);

    ghostState.Position = Vector2{13, 14};
    mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
}

TEST_F(IGhostTest, CalculateChaseTarget_DifferentGhostDirections) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .Times(4)
        .WillRepeatedly(testing::Return(Vector2{0, 0}));

    for (auto dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
        ghostState.CurrentDirection = dir;
        mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
    }
}

TEST_F(IGhostTest, CalculateChaseTarget_DifferentPlayerPositions) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .Times(4)
        .WillRepeatedly(testing::Return(Vector2{0, 0}));

    std::vector<Vector2> positions = {
        {0, 0}, {27, 30}, {13, 26}, {1, 1}
    };

    for (const auto& pos : positions) {
        playerState.Position = pos;
        mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
    }
}

TEST_F(IGhostTest, CalculateChaseTarget_DifferentPlayerDirections) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .Times(5)
        .WillRepeatedly(testing::Return(Vector2{0, 0}));

    for (auto dir : {Direction::Up, Direction::Down, Direction::Left,
                      Direction::Right, Direction::None}) {
        playerState.CurrentDirection = dir;
        mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
    }
}

TEST_F(IGhostTest, CalculateChaseTarget_PoweredUpPlayer) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .Times(2)
        .WillRepeatedly(testing::Return(Vector2{0, 0}));

    playerState.IsPoweredUp = false;
    mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);

    playerState.IsPoweredUp = true;
    mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
}

TEST_F(IGhostTest, CalculateChaseTarget_DifferentBlinkyPositions) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .Times(4)
        .WillRepeatedly(testing::Return(Vector2{0, 0}));

    std::vector<Vector2> blinkyPositions = {
        {0, 0}, {15, 20}, {27, 30}, {5, 5}
    };

    for (const auto& pos : blinkyPositions) {
        mockGhost->CalculateChaseTarget(ghostState, playerState, pos);
    }
}

TEST_F(IGhostTest, GetGhostType_AllTypes) {
    for (auto type : {GhostType::Red, GhostType::Pink,
                       GhostType::Blue, GhostType::Orange}) {
        auto ghost = std::make_unique<MockGhost>();
        EXPECT_CALL(*ghost, GetGhostType())
            .WillOnce(testing::Return(type));

        EXPECT_EQ(ghost->GetGhostType(), type);
    }
}

TEST_F(IGhostTest, GetScatterTarget_DifferentCorners) {
    std::vector<Vector2> corners = {
        {25, -3},
        {2, -3},
        {27, 34},
        {0, 34}
    };

    for (const auto& corner : corners) {
        auto ghost = std::make_unique<MockGhost>();
        EXPECT_CALL(*ghost, GetScatterTarget())
            .WillOnce(testing::Return(corner));

        Vector2 target = ghost->GetScatterTarget();
        EXPECT_EQ(target, corner);
    }
}

TEST_F(IGhostTest, GetScatterTarget_NegativeCoordinates) {
    EXPECT_CALL(*mockGhost, GetScatterTarget())
        .WillOnce(testing::Return(Vector2{25, -3}));

    Vector2 target = mockGhost->GetScatterTarget();
    EXPECT_EQ(target.Y, -3);
}

TEST_F(IGhostTest, GetScatterTarget_OutOfBounds) {
    EXPECT_CALL(*mockGhost, GetScatterTarget())
        .WillOnce(testing::Return(Vector2{27, 34}));

    Vector2 target = mockGhost->GetScatterTarget();
    EXPECT_GT(target.Y, 30);
}

TEST_F(IGhostTest, CalculateChaseTarget_GhostAtPlayerPosition) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(Vector2{13, 26}));

    ghostState.Position = Vector2{13, 26};
    playerState.Position = Vector2{13, 26};

    Vector2 target = mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
    EXPECT_EQ(target, playerState.Position);
}

TEST_F(IGhostTest, CalculateChaseTarget_GhostAtBlinkyPosition) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(Vector2{10, 10}));

    ghostState.Position = Vector2{10, 10};
    blinkyPosition = Vector2{10, 10};

    mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
}

TEST_F(IGhostTest, CalculateChaseTarget_AllAtSamePosition) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(Vector2{13, 13}));

    Vector2 samePos{13, 13};
    ghostState.Position = samePos;
    playerState.Position = samePos;
    blinkyPosition = samePos;

    mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
}

TEST_F(IGhostTest, CalculateChaseTarget_ExtremeDistances) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(Vector2{0, 0}));

    ghostState.Position = Vector2{0, 0};
    playerState.Position = Vector2{27, 30};
    blinkyPosition = Vector2{27, 30};

    mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
}

TEST_F(IGhostTest, CalculateChaseTarget_ReturnsValidCoordinates) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(Vector2{15, 20}));

    Vector2 target = mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);

    EXPECT_GE(target.X, -100);
    EXPECT_LE(target.X, 100);
    EXPECT_GE(target.Y, -100);
    EXPECT_LE(target.Y, 100);
}

TEST_F(IGhostTest, CalculateChaseTarget_CanReturnNegativeCoordinates) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(Vector2{-5, -10}));

    Vector2 target = mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
    EXPECT_LT(target.X, 0);
    EXPECT_LT(target.Y, 0);
}

TEST_F(IGhostTest, CalculateChaseTarget_CanReturnLargeCoordinates) {
    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(Vector2{50, 50}));

    Vector2 target = mockGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
    EXPECT_GT(target.X, 27);
    EXPECT_GT(target.Y, 30);
}

TEST_F(IGhostTest, MethodsAreConst) {
    const IGhost* constGhost = mockGhost.get();

    EXPECT_CALL(*mockGhost, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(Vector2{0, 0}));
    EXPECT_CALL(*mockGhost, GetScatterTarget())
        .WillOnce(testing::Return(Vector2{0, 0}));
    EXPECT_CALL(*mockGhost, GetGhostType())
        .WillOnce(testing::Return(GhostType::Red));

    constGhost->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
    constGhost->GetScatterTarget();
    constGhost->GetGhostType();
}

TEST_F(IGhostTest, VirtualDestructor_AllowsPolymorphicDeletion) {
    IGhost* ghost = new MockGhost();
    EXPECT_NO_THROW(delete ghost);
}

TEST_F(IGhostTest, CreateRedAI_ReturnsNonNull) {
    auto redAI = CreateRedAI();
    EXPECT_NE(redAI, nullptr);
}

TEST_F(IGhostTest, CreatePinkAI_ReturnsNonNull) {
    auto pinkAI = CreatePinkAI();
    EXPECT_NE(pinkAI, nullptr);
}

TEST_F(IGhostTest, CreateBlueAI_ReturnsNonNull) {
    auto blueAI = CreateBlueAI();
    EXPECT_NE(blueAI, nullptr);
}

TEST_F(IGhostTest, CreateOrangeAI_ReturnsNonNull) {
    auto orangeAI = CreateOrangeAI();
    EXPECT_NE(orangeAI, nullptr);
}

TEST_F(IGhostTest, CreateGhostAI_AllTypes_ReturnNonNull) {
    for (auto type : {GhostType::Red, GhostType::Pink,
                       GhostType::Blue, GhostType::Orange}) {
        auto ghostAI = CreateGhostAI(type);
        EXPECT_NE(ghostAI, nullptr);
    }
}

TEST_F(IGhostTest, CreatedGhosts_HaveCorrectType) {
    auto redAI = CreateRedAI();
    EXPECT_EQ(redAI->GetGhostType(), GhostType::Red);

    auto pinkAI = CreatePinkAI();
    EXPECT_EQ(pinkAI->GetGhostType(), GhostType::Pink);

    auto blueAI = CreateBlueAI();
    EXPECT_EQ(blueAI->GetGhostType(), GhostType::Blue);

    auto orangeAI = CreateOrangeAI();
    EXPECT_EQ(orangeAI->GetGhostType(), GhostType::Orange);
}

TEST_F(IGhostTest, CreateGhostAI_MatchesSpecificFactories) {
    auto redAI = CreateGhostAI(GhostType::Red);
    EXPECT_EQ(redAI->GetGhostType(), GhostType::Red);

    auto pinkAI = CreateGhostAI(GhostType::Pink);
    EXPECT_EQ(pinkAI->GetGhostType(), GhostType::Pink);

    auto blueAI = CreateGhostAI(GhostType::Blue);
    EXPECT_EQ(blueAI->GetGhostType(), GhostType::Blue);

    auto orangeAI = CreateGhostAI(GhostType::Orange);
    EXPECT_EQ(orangeAI->GetGhostType(), GhostType::Orange);
}

TEST_F(IGhostTest, MultipleGhostCreation_WorksCorrectly) {
    std::vector<std::unique_ptr<IGhost>> ghosts;
    ghosts.push_back(CreateRedAI());
    ghosts.push_back(CreatePinkAI());
    ghosts.push_back(CreateBlueAI());
    ghosts.push_back(CreateOrangeAI());

    EXPECT_EQ(ghosts.size(), 4);
    for (const auto& ghost : ghosts) {
        EXPECT_NE(ghost, nullptr);
    }
}

TEST_F(IGhostTest, DifferentGhosts_CanHaveDifferentTargets) {
    auto ghost1 = std::make_unique<MockGhost>();
    auto ghost2 = std::make_unique<MockGhost>();

    EXPECT_CALL(*ghost1, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(Vector2{10, 10}));
    EXPECT_CALL(*ghost2, CalculateChaseTarget(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(Vector2{20, 20}));

    Vector2 target1 = ghost1->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
    Vector2 target2 = ghost2->CalculateChaseTarget(ghostState, playerState, blinkyPosition);

    EXPECT_NE(target1, target2);
}

TEST_F(IGhostTest, SameInput_DifferentGhosts_CanDiffer) {
    auto redAI = CreateRedAI();
    auto pinkAI = CreatePinkAI();

    Vector2 redTarget = redAI->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
    Vector2 pinkTarget = pinkAI->CalculateChaseTarget(ghostState, playerState, blinkyPosition);
}

TEST_F(IGhostTest, ScatterTargets_AreDifferentForDifferentGhosts) {
    auto redAI = CreateRedAI();
    auto pinkAI = CreatePinkAI();
    auto blueAI = CreateBlueAI();
    auto orangeAI = CreateOrangeAI();

    Vector2 redScatter = redAI->GetScatterTarget();
    Vector2 pinkScatter = pinkAI->GetScatterTarget();
    Vector2 blueScatter = blueAI->GetScatterTarget();
    Vector2 orangeScatter = orangeAI->GetScatterTarget();

    std::set<Vector2> uniqueTargets;
    uniqueTargets.insert(redScatter);
    uniqueTargets.insert(pinkScatter);
    uniqueTargets.insert(blueScatter);
    uniqueTargets.insert(orangeScatter);

    EXPECT_EQ(uniqueTargets.size(), 4);
}
