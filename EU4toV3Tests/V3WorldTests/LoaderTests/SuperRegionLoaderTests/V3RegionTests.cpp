#include "Loaders/SuperRegionLoader/V3Region.h"
#include "gtest/gtest.h"

TEST(V3World_V3RegionTests, regionCanBeInitialized)
{
	V3::Region region;

	EXPECT_EQ(std::string(), region.getName());
	EXPECT_EQ(0, region.getStates().size());

	std::stringstream input;
	input << "states = { STATE_TEST_1 STATE_TEST_2 }\n";
	region.initializeRegion(input);
	region.setName("test_region");

	EXPECT_EQ("test_region", region.getName());
	EXPECT_EQ(2, region.getStates().size());
	EXPECT_TRUE(region.containsState("STATE_TEST_1"));
	EXPECT_TRUE(region.containsState("STATE_TEST_2"));
	EXPECT_FALSE(region.containsState("STATE_TEST_3"));
}
