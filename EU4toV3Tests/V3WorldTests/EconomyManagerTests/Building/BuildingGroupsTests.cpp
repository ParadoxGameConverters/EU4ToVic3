#include "EconomyManager/Building/BuildingGroups.h"
#include "Loaders/BuildingLoader/BuildingGroupLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
TEST(V3World_BuildingGroupsTests, BadKeyReturnsDefaultValuesAndIsLogged)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	buildingGroupLoader.loadBuildingGroups(modFS);

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

TEST(V3World_BuildingGroupsTests, GettersReturnNullOptionals)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	buildingGroupLoader.loadBuildingGroups(modFS);

	auto parentName = buildingGroups->getParentName("not_a_key0");
	auto infrastructureCost = buildingGroups->getInfrastructureCost("not_a_key1");

	EXPECT_EQ(std::nullopt, parentName);
	EXPECT_EQ(std::nullopt, infrastructureCost);
}