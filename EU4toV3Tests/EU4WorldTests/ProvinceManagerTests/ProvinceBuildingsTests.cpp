#include "ProvinceManager/ProvinceBuildings.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_ProvinceBuildingsTests, buildingsDefaultsToEmpty)
{
	std::stringstream input;
	const EU4::ProvinceBuildings theBuildings(input);

	EXPECT_TRUE(theBuildings.getBuildings().empty());
}

TEST(EU4World_ProvinceBuildingsTests, buildingsCanBeAdded)
{
	std::stringstream input;
	input << "theBuilding=yes\n";
	const EU4::ProvinceBuildings theBuildings(input);

	EXPECT_THAT(theBuildings.getBuildings(), UnorderedElementsAre("theBuilding"));
}
