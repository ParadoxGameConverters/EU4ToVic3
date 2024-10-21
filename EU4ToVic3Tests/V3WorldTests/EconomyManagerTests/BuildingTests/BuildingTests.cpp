#include "EconomyManager/Building/Building.h"
#include "Loaders/DefinesLoader/BuildingScriptValuesLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_BuildingTests, NameCanBeSet)
{
	V3::Building building;

	EXPECT_TRUE(building.getName().empty());

	building.setName("name");

	EXPECT_EQ("name", building.getName());
}

TEST(V3World_SubStateTests, DefaultsDefaultToDefault)
{
	std::map<std::string, int> emptyCostTiers;
	std::stringstream input;

	V3::Building building;
	building.loadBuilding(input, emptyCostTiers);

	EXPECT_TRUE(building.getName().empty());
	EXPECT_TRUE(building.isBuildable());
	EXPECT_FALSE(building.isCappedByGov());
	EXPECT_FALSE(building.isUnique());
	EXPECT_TRUE(building.getUnlockingTechs().empty());
	EXPECT_TRUE(building.getBuildingGroup().empty());
	EXPECT_EQ(50, building.getConstructionCost());
}

TEST(V3World_BuildingTests, ParserLoadsInValues)
{
	V3::BuildingScriptValuesLoader buildingScriptValuesLoader;
	buildingScriptValuesLoader.loadBuildingScriptValues(modFS);
	const auto& costTiers = buildingScriptValuesLoader.getBuildingCostConstants();

	std::stringstream input;
	input << "\tbuilding_group = group_one\n";
	input << "\tbuildable = no\n";
	input << "\tunique = yes\n";
	input << "\thas_max_level = yes\n";
	input << "\tunlocking_technologies = { fire mistake }\n";
	input << "\tproduction_method_groups =  { pmg_group_base pmg_group_secondary }\n";
	input << "\trequired_construction = construction_cost_medium\n";

	V3::Building building;
	building.loadBuilding(input, costTiers);

	EXPECT_THAT("group_one", building.getBuildingGroup());
	EXPECT_FALSE(building.isBuildable());
	EXPECT_TRUE(building.isCappedByGov());
	EXPECT_TRUE(building.isUnique());
	EXPECT_THAT(building.getUnlockingTechs(), testing::UnorderedElementsAre("fire", "mistake"));
	EXPECT_THAT(building.getPMGroups(), testing::UnorderedElementsAre("pmg_group_base", "pmg_group_secondary"));
	EXPECT_EQ(300, building.getConstructionCost());
}

TEST(V3World_BuildingTests, LiteralConstructionCostIsParsed)
{
	V3::BuildingScriptValuesLoader buildingScriptValuesLoader;
	buildingScriptValuesLoader.loadBuildingScriptValues(modFS);
	const auto& costTiers = buildingScriptValuesLoader.getBuildingCostConstants();

	std::stringstream input;
	input << "\tbuilding_group = group_one\n";
	input << "\tunlocking_technologies = { fire }\n";
	input << "\tproduction_method_groups =  { pmg_group_base pmg_group_secondary }\n";
	input << "\trequired_construction = 327\n";

	V3::Building building;
	building.loadBuilding(input, costTiers);

	EXPECT_THAT("group_one", building.getBuildingGroup());
	EXPECT_THAT(building.getUnlockingTechs(), testing::UnorderedElementsAre("fire"));
	EXPECT_THAT(building.getPMGroups(), testing::UnorderedElementsAre("pmg_group_base", "pmg_group_secondary"));
	EXPECT_EQ(327, building.getConstructionCost());
}

TEST(V3World_BuildingTests, UnkownConstructionCostIsCaught)
{
	V3::BuildingScriptValuesLoader buildingScriptValuesLoader;
	buildingScriptValuesLoader.loadBuildingScriptValues(modFS);
	const auto& costTiers = buildingScriptValuesLoader.getBuildingCostConstants();

	std::stringstream input;
	input << "\tbuilding_group = group_one\n";
	input << "\tunlocking_technologies = { fire }\n";
	input << "\tproduction_method_groups =  { pmg_group_base pmg_group_secondary }\n";
	input << "\trequired_construction = a_lot\n";

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	V3::Building building;
	building.loadBuilding(input, costTiers);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([ERROR] Failed to understand building cost a_lot:)"));

	EXPECT_THAT("group_one", building.getBuildingGroup());
	EXPECT_THAT(building.getUnlockingTechs(), testing::UnorderedElementsAre("fire"));
	EXPECT_THAT(building.getPMGroups(), testing::UnorderedElementsAre("pmg_group_base", "pmg_group_secondary"));
	EXPECT_EQ(50, building.getConstructionCost());

	std::cout.rdbuf(cout_buffer);
}