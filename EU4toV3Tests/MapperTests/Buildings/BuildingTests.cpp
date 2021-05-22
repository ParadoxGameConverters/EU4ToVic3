#include "Buildings/Building.h"
#include "gtest/gtest.h"

TEST(Mappers_BuildingTests, costDefaultsToZero)
{
	std::stringstream input;
	const mappers::Building theBuilding(input);

	ASSERT_EQ(0, theBuilding.getCost());
}

TEST(Mappers_BuildingTests, costCanBeSet)
{
	std::stringstream input;
	input << "cost = 100\n";
	const mappers::Building theBuilding(input);

	ASSERT_NEAR(100, theBuilding.getCost(), 0.001);
}
