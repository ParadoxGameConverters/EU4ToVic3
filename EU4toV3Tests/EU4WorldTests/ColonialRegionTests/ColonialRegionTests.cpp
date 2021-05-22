#include "ColonialRegions/ColonialRegion.h"
#include "gtest/gtest.h"

TEST(EU4World_ColonialRegionTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::ColonialRegion region(input);

	ASSERT_TRUE(region.getProvinces().empty());
}

TEST(EU4World_ColonialRegionTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "provinces={\n";
	input << "1 2 3\n";
	input << "}\n";
	const EU4::ColonialRegion region(input);

	ASSERT_EQ(3, region.getProvinces().size());
	ASSERT_TRUE(region.getProvinces().count(1));
	ASSERT_TRUE(region.getProvinces().count(2));
	ASSERT_TRUE(region.getProvinces().count(3));
}
