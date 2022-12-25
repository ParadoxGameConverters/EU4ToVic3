#include "EconomyManager/Building/Building.h"
#include "Loaders/DefinesLoader/BuildingScriptValuesLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
TEST(V3World_BuildingTests, ParserLoadsInValues)
{
	V3::BuildingScriptValuesLoader buildingScriptValuesLoader;
	const auto& costTiers = buildingScriptValuesLoader.getBuildingCostConstants();

	std::stringstream input;
	input << "\tbuilding_group = group_one\n";
	input << "\tunlocking_technologies = { fire mistake }\n";
	input << "\tproduction_method_groups =  { pmg_group_base pmg_group_secondary }\n";
	input << "\trequired_construction = construction_cost_medium\n";

	V3::Building building;
	building.loadBuilding(input, costTiers);

	EXPECT_THAT("group_one", building.getBuildingGroup());
	EXPECT_THAT("fire", building.getPreReqTech());
	EXPECT_THAT(building.getPMGroups(), testing::UnorderedElementsAre("pmg_group_base", "pmg_group_secondary"));
	EXPECT_EQ(200, building.getConstructionCost());
}

TEST(V3World_BuildingTests, LiteralConstructionCostIsParsed)
{
	V3::BuildingScriptValuesLoader buildingScriptValuesLoader;
	const auto& costTiers = buildingScriptValuesLoader.getBuildingCostConstants();

	std::stringstream input;
	input << "\tbuilding_group = group_one\n";
	input << "\tunlocking_technologies = { fire mistake }\n";
	input << "\tproduction_method_groups =  { pmg_group_base pmg_group_secondary }\n";
	input << "\trequired_construction = 327\n";

	V3::Building building;
	building.loadBuilding(input, costTiers);

	EXPECT_THAT("group_one", building.getBuildingGroup());
	EXPECT_THAT("fire", building.getPreReqTech());
	EXPECT_THAT(building.getPMGroups(), testing::UnorderedElementsAre("pmg_group_base", "pmg_group_secondary"));
	EXPECT_EQ(327, building.getConstructionCost());
}

TEST(V3World_BuildingTests, UnkownConstructionCostIsCaught)
{
	V3::BuildingScriptValuesLoader buildingScriptValuesLoader;
	const auto& costTiers = buildingScriptValuesLoader.getBuildingCostConstants();

	std::stringstream input;
	input << "\tbuilding_group = group_one\n";
	input << "\tunlocking_technologies = { fire mistake }\n";
	input << "\tproduction_method_groups =  { pmg_group_base pmg_group_secondary }\n";
	input << "\trequired_construction = a_lot\n";

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	V3::Building building;
	building.loadBuilding(input, costTiers);

	EXPECT_THAT(log.str(),
		 testing::HasSubstr(R"( [ERROR] Failed to understand building cost a_lot:)"));

	EXPECT_THAT("group_one", building.getBuildingGroup());
	EXPECT_THAT("fire", building.getPreReqTech());
	EXPECT_THAT(building.getPMGroups(), testing::UnorderedElementsAre("pmg_group_base", "pmg_group_secondary"));
	EXPECT_EQ(50, building.getConstructionCost());

	std::cout.rdbuf(cout_buffer);
}