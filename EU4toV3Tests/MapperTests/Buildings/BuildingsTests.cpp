#include "Buildings/Buildings.h"
#include "gtest/gtest.h"

TEST(Mappers_BuildingsTests, nonExistentBuildingReturnsNullopt)
{
	std::stringstream input;
	const mappers::Buildings theBuildings(input);

	ASSERT_EQ(std::nullopt, theBuildings.getBuilding("nonBuilding"));
}

TEST(Mappers_BuildingsTests, buildingIsReturned)
{
	std::stringstream input;
	input << "testBuilding = {\n";
	input << "\tcost = 100\n";
	input << "}";
	input << "testBuilding2 = {\n";
	input << "\tcost = 200\n";
	input << "}";
	const mappers::Buildings theBuildings(input);

	ASSERT_NEAR(100, theBuildings.getBuilding("testBuilding")->getCost(), 0.001);
	ASSERT_NEAR(200, theBuildings.getBuilding("testBuilding2")->getCost(), 0.001);
}
