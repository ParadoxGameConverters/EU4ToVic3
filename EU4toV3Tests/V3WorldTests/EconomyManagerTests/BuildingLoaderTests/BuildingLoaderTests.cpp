#include "EconomyManager/BuildingLoader/BuildingGroupLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderCanLoadBuildingGroups)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	EXPECT_EQ(buildingGroups->getBuildingGroupMap().size(), 0);
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");
	EXPECT_EQ(buildingGroups->getBuildingGroupMap().size(), 6);
}

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderTracksHierarchy)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");

	EXPECT_TRUE(buildingGroups->getParent("bg_manufacturing").empty());
	EXPECT_EQ(buildingGroups->getParent("bg_light_industry"), "bg_manufacturing");
	EXPECT_EQ(buildingGroups->getParent("bg_heavy_industry"), "bg_manufacturing");
	EXPECT_EQ(buildingGroups->getParent("bg_ultra_industry"), "bg_heavy_industry");
	EXPECT_EQ(buildingGroups->getParent("bg_mega_industry"), "bg_ultra_industry");
	EXPECT_EQ(buildingGroups->getParent("bg_giga_industry"), "bg_mega_industry");
}

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderSetsInfrastructureInheritance)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");

	EXPECT_EQ(buildingGroups->getInfrastructureCost("bg_manufacturing"), 0);
	EXPECT_EQ(buildingGroups->getInfrastructureCost("bg_light_industry"), 2);
	EXPECT_EQ(buildingGroups->getInfrastructureCost("bg_heavy_industry"), 3);
	EXPECT_EQ(buildingGroups->getInfrastructureCost("bg_ultra_industry"), 1);
	EXPECT_EQ(buildingGroups->getInfrastructureCost("bg_mega_industry"), 1);
	EXPECT_EQ(buildingGroups->getInfrastructureCost("bg_giga_industry"), 1);
}

TEST(V3World_BuildingGroupLoaderTests, BadKeyReturnsDefaultValuesAndIsLogged)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	const std::string& parentName = buildingGroups->getParent("not_a_key0");
	const int infrastructureCost = buildingGroups->getInfrastructureCost("not_a_key1");

	EXPECT_THAT(log.str(),
		 testing::HasSubstr(R"( [ERROR] Key not recognized: not_a_key0 is not a recognized building_group. Using an empty group in its place.)"));
	EXPECT_THAT(log.str(),
		 testing::HasSubstr(R"( [ERROR] Key not recognized: not_a_key1 is not a recognized building_group. Using an empty group in its place.)"));
	EXPECT_TRUE(parentName.empty());
	EXPECT_EQ(infrastructureCost, 0);

	std::cout.rdbuf(cout_buffer);
}
