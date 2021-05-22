#include "Provinces/ProvinceBuildings.h"
#include "gtest/gtest.h"

TEST(EU4World_Province_BuildingsTests, buildingsDefaultsToEmpty)
{
	std::stringstream input;
	const EU4::ProvinceBuildings theBuildings(input);

	ASSERT_TRUE(theBuildings.getBuildings().empty());
}

TEST(EU4World_Province_BuildingsTests, buildingsCanBeAdded)
{
	std::stringstream input;
	input << "theBuilding=yes\n";
	const EU4::ProvinceBuildings theBuildings(input);

	ASSERT_TRUE(theBuildings.getBuildings().count("theBuilding"));
}

TEST(EU4World_Province_BuildingsTests, nonMatchingBuildingReturnsFalse)
{
	std::stringstream input;
	const EU4::ProvinceBuildings theBuildings(input);

	ASSERT_FALSE(theBuildings.hasBuilding("theBuilding"));
}

TEST(EU4World_Province_BuildingsTests, matchingBuildingReturnsTrue)
{
	std::stringstream input;
	input << "theBuilding=1600.3.2\n";
	const EU4::ProvinceBuildings theBuildings(input);

	ASSERT_TRUE(theBuildings.hasBuilding("theBuilding"));
}
