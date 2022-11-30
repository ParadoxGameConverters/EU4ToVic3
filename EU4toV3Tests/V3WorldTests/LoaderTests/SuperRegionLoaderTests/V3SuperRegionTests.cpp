#include "Loaders/SuperRegionLoader/V3SuperRegion.h"
#include "gtest/gtest.h"

TEST(V3World_V3SuperRegionTests, superRegionCanBeInitialized)
{
	V3::SuperRegion superRegion;

	EXPECT_EQ(std::string(), superRegion.getName());
	EXPECT_EQ(0, superRegion.getRegions().size());

	std::stringstream input;
	input << "region_a = {\n";
	input << "	states = { STATE_TEST_1 STATE_TEST_2 }\n";
	input << "}\n";
	input << "region_b = {\n";
	input << "	states = { STATE_TEST_3 }\n";
	input << "}\n";
	superRegion.initializeSuperRegion(input);
	superRegion.setName("test_superregion");

	EXPECT_EQ("test_superregion", superRegion.getName());
	EXPECT_EQ(2, superRegion.getRegions().size());
	EXPECT_TRUE(superRegion.containsRegion("region_a"));
	EXPECT_TRUE(superRegion.containsRegion("region_b"));
	EXPECT_FALSE(superRegion.containsRegion("region_c"));
}
