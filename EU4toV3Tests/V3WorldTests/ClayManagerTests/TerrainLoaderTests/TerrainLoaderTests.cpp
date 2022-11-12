#include "ClayManager/TerrainLoader/TerrainLoader.h"
#include "gtest/gtest.h"

TEST(V3World_TerrainLoaderTests, terrainLoaderCanLoadTerrains)
{
	V3::TerrainLoader terrainLoader;

	EXPECT_EQ(0, terrainLoader.getTerrains().size());

	terrainLoader.loadTerrains("TestFiles/vic3installation/game/");

	EXPECT_EQ(10, terrainLoader.getTerrains().size());

	EXPECT_EQ("desert", terrainLoader.getTerrains().at("x112233"));
	EXPECT_EQ("plains", terrainLoader.getTerrains().at("x445566"));
	EXPECT_EQ("plains", terrainLoader.getTerrains().at("x778899"));
	EXPECT_EQ("lakes", terrainLoader.getTerrains().at("xAABBCC"));
	EXPECT_EQ("ocean", terrainLoader.getTerrains().at("xDDEEFF"));
	EXPECT_EQ("wetland", terrainLoader.getTerrains().at("x123456"));
	EXPECT_EQ("mountain", terrainLoader.getTerrains().at("x7890AB"));
	EXPECT_EQ("tundra", terrainLoader.getTerrains().at("xCDEF12"));
	EXPECT_EQ("ocean", terrainLoader.getTerrains().at("x345678"));
	EXPECT_EQ("lakes", terrainLoader.getTerrains().at("x9ABCDE"));
}
