#include "BuildingCostLoader/BuildingCostLoader.h"
#include "gtest/gtest.h"

TEST(EU4World_BuildingCostLoaderTests, nonExistentBuildingReturnsNullopt)
{
	std::stringstream input;
	EU4::BuildingCostLoader theBuildings;
	theBuildings.loadBuildingCosts(input);

	EXPECT_EQ(std::nullopt, theBuildings.getBuildingCost("nonBuilding"));
}

TEST(EU4World_BuildingCostLoaderTests, buildingIsReturned)
{
	std::stringstream input;
	input << "testBuilding = {\n";
	input << "\tcost = 100\n";
	input << "}";
	input << "testBuilding2 = {\n";
	input << "\tcost = 200\n";
	input << "}";
	EU4::BuildingCostLoader theBuildings;
	theBuildings.loadBuildingCosts(input);

	EXPECT_NEAR(100, *theBuildings.getBuildingCost("testBuilding"), 0.001);
	EXPECT_NEAR(200, *theBuildings.getBuildingCost("testBuilding2"), 0.001);
}
