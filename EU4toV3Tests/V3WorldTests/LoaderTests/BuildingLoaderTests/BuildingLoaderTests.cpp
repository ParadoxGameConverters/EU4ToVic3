#include "Loaders/BuildingLoader/BuildingGroupLoader.h"
#include "Loaders/BuildingLoader/BuildingGroups.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderCanLoadBuildingGroups)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	EXPECT_TRUE(buildingGroups->getBuildingGroupMap().empty());
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");
	EXPECT_EQ(6, buildingGroups->getBuildingGroupMap().size());
}

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderTracksHierarchy)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");

	EXPECT_TRUE(buildingGroups->getParentName("bg_manufacturing")->empty());
	EXPECT_EQ("bg_manufacturing", buildingGroups->getParentName("bg_light_industry").value());
	EXPECT_EQ("bg_manufacturing", buildingGroups->getParentName("bg_heavy_industry").value());
	EXPECT_EQ("bg_heavy_industry", buildingGroups->getParentName("bg_ultra_industry").value());
	EXPECT_EQ("bg_ultra_industry", buildingGroups->getParentName("bg_mega_industry").value());
	EXPECT_EQ("bg_mega_industry", buildingGroups->getParentName("bg_giga_industry").value());
}

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderSetsInfrastructureInheritance)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");

	EXPECT_EQ(0, buildingGroups->getInfrastructureCost("bg_manufacturing").value());
	EXPECT_EQ(2, buildingGroups->getInfrastructureCost("bg_light_industry").value());
	EXPECT_EQ(3, buildingGroups->getInfrastructureCost("bg_heavy_industry").value());
	EXPECT_EQ(1, buildingGroups->getInfrastructureCost("bg_ultra_industry").value());
	EXPECT_EQ(1, buildingGroups->getInfrastructureCost("bg_mega_industry").value());
	EXPECT_EQ(1, buildingGroups->getInfrastructureCost("bg_giga_industry").value());
}

TEST(V3World_BuildingGroupLoaderTests, BadKeyReturnsDefaultValuesAndIsLogged)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	const std::string& parentName = buildingGroups->safeGetParentName("not_a_key0");
	const int infrastructureCost = buildingGroups->safeGetInfrastructureCost("not_a_key1");

	EXPECT_THAT(log.str(),
		 testing::HasSubstr(R"( [ERROR] Key not recognized: not_a_key0 is not a recognized building_group. Using an empty group in its place.)"));
	EXPECT_THAT(log.str(),
		 testing::HasSubstr(R"( [ERROR] Key not recognized: not_a_key1 is not a recognized building_group. Using an empty group in its place.)"));
	EXPECT_TRUE(parentName.empty());
	EXPECT_EQ(0, infrastructureCost);

	std::cout.rdbuf(cout_buffer);
}

TEST(V3World_BuildingGroupLoaderTests, GettersReturnNullOptionals)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");

	auto parentName = buildingGroups->getParentName("not_a_key0");
	auto infrastructureCost = buildingGroups->getInfrastructureCost("not_a_key1");

	EXPECT_EQ(std::nullopt, parentName);
	EXPECT_EQ(std::nullopt, infrastructureCost);
}
