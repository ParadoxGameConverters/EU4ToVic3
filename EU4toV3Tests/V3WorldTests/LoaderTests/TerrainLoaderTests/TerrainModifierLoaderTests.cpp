#include "Loaders/TerrainLoader/TerrainModifierLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>


TEST(V3World_TerrainModifierLoaderTests, terrainModifierLoaderCanLoadTerrainEffects)
{
	V3::TerrainModifierLoader terrainModifierLoader;

	std::stringstream input;

	input << "coastal = {\n";
	input << "\tpriority = 1.2";
	input << "\tbuilding_cave = 1.3";
	input << "\tbuilding_fish = 2.5";
	input << "}";
	input << "desert = {\n";
	input << "\tpriority = 0.7";
	input << "}";
	input << "wetlands = {\n";
	input << "\tbuilding_fish = 2";
	input << "}";

	EXPECT_EQ(0, terrainModifierLoader.getTerrainStateModifiers().size());
	EXPECT_EQ(0, terrainModifierLoader.getTerrainBuildingModifiers().size());

	terrainModifierLoader.loadTerrainModifiers(input);

	EXPECT_EQ(2, terrainModifierLoader.getTerrainStateModifiers().size());
	EXPECT_EQ(2, terrainModifierLoader.getTerrainBuildingModifiers().size());

	EXPECT_THAT(terrainModifierLoader.getTerrainStateModifiers(), testing::UnorderedElementsAre(testing::Pair("coastal", 1.2), testing::Pair("desert", 0.7)));

	EXPECT_THAT(terrainModifierLoader.getTerrainBuildingModifiers().at("coastal"),
		 testing::UnorderedElementsAre(testing::Pair("building_cave", 1.3), testing::Pair("building_fish", 2.5)));
	EXPECT_THAT(terrainModifierLoader.getTerrainBuildingModifiers().at("wetlands"), testing::UnorderedElementsAre(testing::Pair("building_fish", 2.0)));
}