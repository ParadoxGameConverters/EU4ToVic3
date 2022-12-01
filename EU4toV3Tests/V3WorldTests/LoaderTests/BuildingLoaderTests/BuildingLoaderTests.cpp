#include "Loaders/BuildingLoader/BuildingGroupLoader.h"
#include "Loaders/BuildingLoader/BuildingGroups.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderCanLoadBuildingGroups)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	EXPECT_EQ(buildingGroups->getBuildingGroupMap().size(), 0);
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");
	EXPECT_EQ(buildingGroups->getBuildingGroupMap().size(), 6);
}

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderTracksHierarchy)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");

	EXPECT_TRUE(buildingGroups->getParentName("bg_manufacturing")->empty());
	EXPECT_EQ(buildingGroups->getParentName("bg_light_industry").value(), "bg_manufacturing");
	EXPECT_EQ(buildingGroups->getParentName("bg_heavy_industry").value(), "bg_manufacturing");
	EXPECT_EQ(buildingGroups->getParentName("bg_ultra_industry").value(), "bg_heavy_industry");
	EXPECT_EQ(buildingGroups->getParentName("bg_mega_industry").value(), "bg_ultra_industry");
	EXPECT_EQ(buildingGroups->getParentName("bg_giga_industry").value(), "bg_mega_industry");
}

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderSetsInfrastructureInheritance)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");

	EXPECT_EQ(buildingGroups->getInfrastructureCost("bg_manufacturing").value(), 0);
	EXPECT_EQ(buildingGroups->getInfrastructureCost("bg_light_industry").value(), 2);
	EXPECT_EQ(buildingGroups->getInfrastructureCost("bg_heavy_industry").value(), 3);
	EXPECT_EQ(buildingGroups->getInfrastructureCost("bg_ultra_industry").value(), 1);
	EXPECT_EQ(buildingGroups->getInfrastructureCost("bg_mega_industry").value(), 1);
	EXPECT_EQ(buildingGroups->getInfrastructureCost("bg_giga_industry").value(), 1);
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
	EXPECT_EQ(infrastructureCost, 0);

	std::cout.rdbuf(cout_buffer);
}

TEST(V3World_BuildingGroupLoaderTests, GettersReturnNullOptionals)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	auto parentName = buildingGroups->getParentName("not_a_key0");
	auto infrastructureCost = buildingGroups->getInfrastructureCost("not_a_key1");

	EXPECT_THAT(log.str(), testing::Not(testing::HasSubstr(R"([ERROR])")));
	EXPECT_EQ(parentName, std::nullopt);
	EXPECT_EQ(infrastructureCost, std::nullopt);

	std::cout.rdbuf(cout_buffer);
}
