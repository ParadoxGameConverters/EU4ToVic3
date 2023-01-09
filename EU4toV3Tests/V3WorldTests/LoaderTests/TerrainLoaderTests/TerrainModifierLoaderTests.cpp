#include "Loaders/TerrainLoader/TerrainModifierLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>


TEST(V3World_TerrainModifierLoaderTests, terrainModifierLoaderCanLoadTerrainEffects)
{
	V3::TerrainModifierLoader terrainModifierLoader;

	std::stringstream input;

	input << "coastal = {\n";
	input << "\tpriority = 0.2";
	input << "\tbuilding_cave = 0.3";
	input << "\tbuilding_fish = 1.5";
	input << "}";
	input << "desert = {\n";
	input << "\tpriority = -0.3";
	input << "}";
	input << "wetlands = {\n";
	input << "\tbuilding_fish = 1";
	input << "}";

	EXPECT_EQ(0, terrainModifierLoader.getTerrainStateModifiers().size());
	EXPECT_EQ(0, terrainModifierLoader.getTerrainBuildingModifiers().size());

	terrainModifierLoader.loadTerrainModifiers(input);

	EXPECT_EQ(3, terrainModifierLoader.getTerrainStateModifiers().size());
	EXPECT_EQ(2, terrainModifierLoader.getTerrainBuildingModifiers().size());

	EXPECT_THAT(terrainModifierLoader.getTerrainStateModifiers(),
		 testing::UnorderedElementsAre(testing::Pair("coastal", 0.2), testing::Pair("desert", -0.3), testing::Pair("wetlands", 0)));

	EXPECT_THAT(terrainModifierLoader.getTerrainBuildingModifiers().at("coastal"),
		 testing::UnorderedElementsAre(testing::Pair("building_cave", 0.3), testing::Pair("building_fish", 1.5)));
	EXPECT_THAT(terrainModifierLoader.getTerrainBuildingModifiers().at("wetlands"), testing::UnorderedElementsAre(testing::Pair("building_fish", 1.0)));
}