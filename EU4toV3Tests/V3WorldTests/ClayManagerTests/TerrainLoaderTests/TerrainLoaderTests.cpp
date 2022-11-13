#include "ClayManager/TerrainLoader/TerrainLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_TerrainLoaderTests, terrainLoaderCanLoadTerrains)
{
	V3::TerrainLoader terrainLoader;

	EXPECT_EQ(0, terrainLoader.getTerrains().size());

	terrainLoader.loadTerrains("TestFiles/vic3installation/game/");

	EXPECT_EQ(12, terrainLoader.getTerrains().size());

	EXPECT_THAT(terrainLoader.getTerrains(),
		 testing::UnorderedElementsAre(testing::Pair("x112233", "desert"),
			  testing::Pair("x445566", "plains"),
			  testing::Pair("x778899", "plains"),
			  testing::Pair("xAABBCC", "lakes"),
			  testing::Pair("xDDEEFF", "ocean"),
			  testing::Pair("x123456", "wetland"),
			  testing::Pair("x7890AB", "mountain"),
			  testing::Pair("xCDEF12", "tundra"),
			  testing::Pair("x9ABCDE", "lakes"),
			  testing::Pair("X1111111", "test"),
			  testing::Pair("X11111", "test"),
			  testing::Pair("0X654321", "test")));
}

TEST(V3World_TerrainLoaderTests, terrainLoaderUppercasesProvinceNames)
{
	V3::TerrainLoader terrainLoader;

	EXPECT_EQ(0, terrainLoader.getTerrains().size());

	terrainLoader.loadTerrains("TestFiles/vic3installation/game/");

	EXPECT_TRUE(terrainLoader.getTerrains().contains("x9ABCDE")); // x9abcde in the file
}

TEST(V3World_TerrainLoaderTests, terrainLoaderComplainsAtInvalidNames)
{
	V3::TerrainLoader terrainLoader;

	EXPECT_EQ(0, terrainLoader.getTerrains().size());

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	terrainLoader.loadTerrains("TestFiles/vic3installation/game/");

	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Encountered terrain for province X1111111 in unknown format!)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Encountered terrain for province X11111 in unknown format!)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Encountered terrain for province 0X654321 in unknown format!)"));

	std::cout.rdbuf(cout_buffer);
}
