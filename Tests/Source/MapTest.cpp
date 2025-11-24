#include <gtest/gtest.h>
#include "Map.hpp"

using namespace Pacman;

class MapTest : public ::testing::Test {
protected:
    void SetUp() override {
        map = std::make_unique<Map>();
    }

    std::unique_ptr<Map> map;
};

TEST_F(MapTest, Initialize_SetsCorrectDimensions) {
    EXPECT_EQ(map->GetWidth(), 28);
    EXPECT_EQ(map->GetHeight(), 31);
}

TEST_F(MapTest, GetSize_ReturnsCorrectVector) {
    Vector2 size = map->GetSize();
    EXPECT_EQ(size.X, 28);
    EXPECT_EQ(size.Y, 31);
}

TEST_F(MapTest, Initialize_CountsPellets) {
    int pelletCount = map->GetPelletCount();
    EXPECT_GT(pelletCount, 0);
    EXPECT_EQ(pelletCount, map->GetInitialPelletCount());
}

TEST_F(MapTest, Initialize_CornerTilesAreWalls) {
    EXPECT_EQ(map->GetTileAt({0, 0}), TileType::Wall);
    EXPECT_EQ(map->GetTileAt({27, 0}), TileType::Wall);
    EXPECT_EQ(map->GetTileAt({0, 30}), TileType::Wall);
    EXPECT_EQ(map->GetTileAt({27, 30}), TileType::Wall);
}

TEST_F(MapTest, Initialize_HasPellets) {
    EXPECT_EQ(map->GetTileAt({1, 1}), TileType::Pellet);
}

TEST_F(MapTest, Initialize_HasPowerPellets) {
    EXPECT_EQ(map->GetTileAt({1, 3}), TileType::PowerPellet);
}

TEST_F(MapTest, Initialize_HasGhostDoor) {
    bool foundGhostDoor = false;
    for (int y = 0; y < map->GetHeight(); ++y) {
        for (int x = 0; x < map->GetWidth(); ++x) {
            if (map->GetTileAt({x, y}) == TileType::GhostDoor) {
                foundGhostDoor = true;
                break;
            }
        }
    }
    EXPECT_TRUE(foundGhostDoor);
}

TEST_F(MapTest, ReInitialize_RestoresPellets) {
    Vector2 pelletPos{1, 1};
    int initialCount = map->GetPelletCount();

    map->SetTileAt(pelletPos, TileType::Empty);
    EXPECT_LT(map->GetPelletCount(), initialCount);

    map->Initialize();
    EXPECT_EQ(map->GetPelletCount(), initialCount);
    EXPECT_EQ(map->GetTileAt(pelletPos), TileType::Pellet);
}

TEST_F(MapTest, IsInBounds_ValidPositions) {
    EXPECT_TRUE(map->IsInBounds({0, 0}));
    EXPECT_TRUE(map->IsInBounds({27, 30}));
    EXPECT_TRUE(map->IsInBounds({13, 15}));
}

TEST_F(MapTest, IsInBounds_NegativeX) {
    EXPECT_FALSE(map->IsInBounds({-1, 0}));
}

TEST_F(MapTest, IsInBounds_NegativeY) {
    EXPECT_FALSE(map->IsInBounds({0, -1}));
}

TEST_F(MapTest, IsInBounds_XTooLarge) {
    EXPECT_FALSE(map->IsInBounds({28, 0}));
}

TEST_F(MapTest, IsInBounds_YTooLarge) {
    EXPECT_FALSE(map->IsInBounds({0, 31}));
}

TEST_F(MapTest, IsInBounds_BothNegative) {
    EXPECT_FALSE(map->IsInBounds({-5, -10}));
}

TEST_F(MapTest, IsInBounds_BothTooLarge) {
    EXPECT_FALSE(map->IsInBounds({100, 100}));
}

TEST_F(MapTest, GetTileAt_OutOfBounds_ReturnsWall) {
    EXPECT_EQ(map->GetTileAt({-1, 0}), TileType::Wall);
    EXPECT_EQ(map->GetTileAt({0, -1}), TileType::Wall);
    EXPECT_EQ(map->GetTileAt({28, 0}), TileType::Wall);
    EXPECT_EQ(map->GetTileAt({0, 31}), TileType::Wall);
}

TEST_F(MapTest, SetTileAt_ChangesRegularTile) {
    Vector2 pos{5, 5};
    TileType original = map->GetTileAt(pos);

    map->SetTileAt(pos, TileType::Empty);
    EXPECT_EQ(map->GetTileAt(pos), TileType::Empty);

    map->SetTileAt(pos, original);
    EXPECT_EQ(map->GetTileAt(pos), original);
}

TEST_F(MapTest, SetTileAt_EatingPellet_DecrementsPelletCount) {
    Vector2 pelletPos{1, 1};
    EXPECT_EQ(map->GetTileAt(pelletPos), TileType::Pellet);

    int initialCount = map->GetPelletCount();
    map->SetTileAt(pelletPos, TileType::Empty);

    EXPECT_EQ(map->GetPelletCount(), initialCount - 1);
    EXPECT_EQ(map->GetTileAt(pelletPos), TileType::Empty);
}

TEST_F(MapTest, SetTileAt_EatingPowerPellet_DecrementsPelletCount) {
    Vector2 powerPelletPos{1, 3};
    EXPECT_EQ(map->GetTileAt(powerPelletPos), TileType::PowerPellet);

    int initialCount = map->GetPelletCount();
    map->SetTileAt(powerPelletPos, TileType::Empty);

    EXPECT_EQ(map->GetPelletCount(), initialCount - 1);
    EXPECT_EQ(map->GetTileAt(powerPelletPos), TileType::Empty);
}

TEST_F(MapTest, SetTileAt_ChangingNonPelletTile_DoesNotAffectPelletCount) {
    Vector2 wallPos{0, 0};
    int initialCount = map->GetPelletCount();

    map->SetTileAt(wallPos, TileType::Path);

    EXPECT_EQ(map->GetPelletCount(), initialCount);
}

TEST_F(MapTest, SetTileAt_OutOfBounds_DoesNothing) {
    int initialCount = map->GetPelletCount();
    map->SetTileAt({-1, 0}, TileType::Empty);
    map->SetTileAt({28, 0}, TileType::Empty);
    EXPECT_EQ(map->GetPelletCount(), initialCount);
}

TEST_F(MapTest, SetTileAt_ChangingPelletToPath_DecrementsPelletCount) {
    Vector2 pelletPos{1, 1};
    int initialCount = map->GetPelletCount();

    map->SetTileAt(pelletPos, TileType::Path);

    EXPECT_EQ(map->GetPelletCount(), initialCount - 1);
}

TEST_F(MapTest, IsWalkable_PathTile_ReturnsTrue) {
    Vector2 pathPos{13, 14};
    if (map->GetTileAt(pathPos) == TileType::Path) {
        EXPECT_TRUE(map->IsWalkable(pathPos));
    }
}

TEST_F(MapTest, IsWalkable_PelletTile_ReturnsTrue) {
    Vector2 pelletPos{1, 1};
    EXPECT_TRUE(map->IsWalkable(pelletPos));
}

TEST_F(MapTest, IsWalkable_PowerPelletTile_ReturnsTrue) {
    Vector2 powerPelletPos{1, 3};
    EXPECT_TRUE(map->IsWalkable(powerPelletPos));
}

TEST_F(MapTest, IsWalkable_WallTile_ReturnsFalse) {
    Vector2 wallPos{0, 0};
    EXPECT_FALSE(map->IsWalkable(wallPos));
}

TEST_F(MapTest, IsWalkable_GhostDoorTile_ReturnsFalse) {
    for (int y = 0; y < map->GetHeight(); ++y) {
        for (int x = 0; x < map->GetWidth(); ++x) {
            if (map->GetTileAt({x, y}) == TileType::GhostDoor) {
                EXPECT_FALSE(map->IsWalkable({x, y}));
                return;
            }
        }
    }
}

TEST_F(MapTest, IsWalkable_EmptyTile_ReturnsTrue) {
    Vector2 pos{5, 5};
    map->SetTileAt(pos, TileType::Empty);
    EXPECT_TRUE(map->IsWalkable(pos));
}

TEST_F(MapTest, IsGhostWalkable_WallTile_ReturnsFalse) {
    Vector2 wallPos{0, 0};
    EXPECT_FALSE(map->IsGhostWalkable(wallPos, false));
    EXPECT_FALSE(map->IsGhostWalkable(wallPos, true));
}

TEST_F(MapTest, IsGhostWalkable_PathTile_ReturnsTrue) {
    Vector2 pos{13, 14};
    if (map->GetTileAt(pos) == TileType::Path) {
        EXPECT_TRUE(map->IsGhostWalkable(pos, false));
        EXPECT_TRUE(map->IsGhostWalkable(pos, true));
    }
}

TEST_F(MapTest, IsGhostWalkable_GhostDoor_WithoutPermission_ReturnsFalse) {
    for (int y = 0; y < map->GetHeight(); ++y) {
        for (int x = 0; x < map->GetWidth(); ++x) {
            if (map->GetTileAt({x, y}) == TileType::GhostDoor) {
                EXPECT_FALSE(map->IsGhostWalkable({x, y}, false));
                return;
            }
        }
    }
}

TEST_F(MapTest, IsGhostWalkable_GhostDoor_WithPermission_ReturnsTrue) {
    for (int y = 0; y < map->GetHeight(); ++y) {
        for (int x = 0; x < map->GetWidth(); ++x) {
            if (map->GetTileAt({x, y}) == TileType::GhostDoor) {
                EXPECT_TRUE(map->IsGhostWalkable({x, y}, true));
                return;
            }
        }
    }
}

TEST_F(MapTest, IsGhostWalkable_PelletTile_ReturnsTrue) {
    Vector2 pelletPos{1, 1};
    EXPECT_TRUE(map->IsGhostWalkable(pelletPos, false));
    EXPECT_TRUE(map->IsGhostWalkable(pelletPos, true));
}

TEST_F(MapTest, WrapPosition_NegativeX_WrapsToRight) {
    Vector2 pos{-1, 15};
    Vector2 wrapped = map->WrapPosition(pos);
    EXPECT_EQ(wrapped.X, 27);
    EXPECT_EQ(wrapped.Y, 15);
}

TEST_F(MapTest, WrapPosition_XTooLarge_WrapsToLeft) {
    Vector2 pos{28, 15};
    Vector2 wrapped = map->WrapPosition(pos);
    EXPECT_EQ(wrapped.X, 0);
    EXPECT_EQ(wrapped.Y, 15);
}

TEST_F(MapTest, WrapPosition_ValidX_NoChange) {
    Vector2 pos{13, 15};
    Vector2 wrapped = map->WrapPosition(pos);
    EXPECT_EQ(wrapped.X, 13);
    EXPECT_EQ(wrapped.Y, 15);
}

TEST_F(MapTest, WrapPosition_YNotWrapped) {
    Vector2 pos{0, 100};
    Vector2 wrapped = map->WrapPosition(pos);
    EXPECT_EQ(wrapped.Y, 100);
}

TEST_F(MapTest, WrapPosition_MultipleWraps) {
    Vector2 pos{-28, 15};
    Vector2 wrapped = map->WrapPosition(pos);
    EXPECT_EQ(wrapped.X, 0);
}

TEST_F(MapTest, GetPelletPositions_ReturnsAllPellets) {
    std::vector<Vector2> positions = map->GetPelletPositions();
    EXPECT_EQ(static_cast<int>(positions.size()), map->GetPelletCount());
}

TEST_F(MapTest, GetPelletPositions_IncludesPowerPellets) {
    std::vector<Vector2> positions = map->GetPelletPositions();

    bool foundPowerPellet = false;
    for (const auto& pos : positions) {
        if (map->GetTileAt(pos) == TileType::PowerPellet) {
            foundPowerPellet = true;
            break;
        }
    }
    EXPECT_TRUE(foundPowerPellet);
}

TEST_F(MapTest, GetPelletPositions_OnlyReturnsValidPellets) {
    std::vector<Vector2> positions = map->GetPelletPositions();

    for (const auto& pos : positions) {
        TileType tile = map->GetTileAt(pos);
        EXPECT_TRUE(tile == TileType::Pellet || tile == TileType::PowerPellet);
    }
}

TEST_F(MapTest, GetPelletPositions_UpdatesAfterEating) {
    Vector2 pelletPos{1, 1};
    map->SetTileAt(pelletPos, TileType::Empty);

    std::vector<Vector2> positions = map->GetPelletPositions();
    EXPECT_EQ(static_cast<int>(positions.size()), map->GetPelletCount());

    bool foundEatenPellet = false;
    for (const auto& pos : positions) {
        if (pos == pelletPos) {
            foundEatenPellet = true;
            break;
        }
    }
    EXPECT_FALSE(foundEatenPellet);
}

TEST_F(MapTest, ConsecutivePelletEating) {
    Vector2 pelletPos1{1, 1};
    Vector2 pelletPos2{2, 1};

    int initialCount = map->GetPelletCount();

    map->SetTileAt(pelletPos1, TileType::Empty);
    EXPECT_EQ(map->GetPelletCount(), initialCount - 1);

    map->SetTileAt(pelletPos2, TileType::Empty);
    EXPECT_EQ(map->GetPelletCount(), initialCount - 2);
}

TEST_F(MapTest, AllPelletsCanBeEaten) {
    int initialCount = map->GetPelletCount();
    std::vector<Vector2> positions = map->GetPelletPositions();

    for (const auto& pos : positions) {
        map->SetTileAt(pos, TileType::Empty);
    }

    EXPECT_EQ(map->GetPelletCount(), 0);
    EXPECT_EQ(initialCount, map->GetInitialPelletCount());
}

TEST_F(MapTest, GetInitialPelletCount_RemainsConstant) {
    int initial = map->GetInitialPelletCount();

    map->SetTileAt({1, 1}, TileType::Empty);
    map->SetTileAt({2, 1}, TileType::Empty);

    EXPECT_EQ(map->GetInitialPelletCount(), initial);
}
