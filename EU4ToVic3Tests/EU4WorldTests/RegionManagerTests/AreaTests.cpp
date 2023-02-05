#include "RegionManager/Area.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_AreaTests, blankAreasLoadForNoProvinces)
{
	std::stringstream input;
	const EU4::Area newArea(input);

	EXPECT_TRUE(newArea.getProvinces().empty());
}

TEST(Mappers_AreaTests, provincesCanBeLoaded)
{
	std::stringstream input;
	input << "{ 1 2 3 }";
	const EU4::Area newArea(input);

	EXPECT_THAT(newArea.getProvinces(), testing::UnorderedElementsAre(1, 2, 3));
}

TEST(Mappers_AreaTests, provincesCanBeFound)
{
	std::stringstream input;
	input << "{ 1 2 3 }";
	const EU4::Area newArea(input);

	EXPECT_TRUE(newArea.areaContainsProvince(1));
	EXPECT_TRUE(newArea.areaContainsProvince(2));
	EXPECT_TRUE(newArea.areaContainsProvince(3));
}

TEST(Mappers_AreaTests, provinceMismatchReturnsFalse)
{
	std::stringstream input;
	input << "{ 1 2 3 }";
	const EU4::Area newArea(input);

	EXPECT_FALSE(newArea.areaContainsProvince(4));
}
