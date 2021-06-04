#include "ColonialRegionLoader/ColonialRegion.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_ColonialRegionTests, provincesDefaultToBlank)
{
	std::stringstream input;
	const EU4::ColonialRegion region(input);

	EXPECT_TRUE(region.getProvinces().empty());
}

TEST(EU4World_ColonialRegionTests, provincesCanBeLoaded)
{
	std::stringstream input;
	input << "provinces={\n";
	input << "	1 2 3\n";
	input << "}\n";
	const EU4::ColonialRegion region(input);

	EXPECT_THAT(region.getProvinces(), UnorderedElementsAre(1, 2, 3));
}
