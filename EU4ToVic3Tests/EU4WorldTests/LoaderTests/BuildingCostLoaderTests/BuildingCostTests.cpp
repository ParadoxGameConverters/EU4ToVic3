#include "BuildingCostLoader/BuildingCost.h"
#include "gtest/gtest.h"

TEST(EU4World_BuildingCostTests, costDefaultsToZero)
{
	std::stringstream input;
	const EU4::BuildingCost theBuilding(input);

	EXPECT_EQ(0, theBuilding.getCost());
}

TEST(EU4World_BuildingCostTests, costCanBeSet)
{
	std::stringstream input;
	input << "cost = 100\n";
	const EU4::BuildingCost theBuilding(input);

	EXPECT_NEAR(100, theBuilding.getCost(), 0.001);
}
