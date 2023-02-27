#include "Loaders/SuperRegionLoader/SuperRegionLoader.h"
#include "Loaders/SuperRegionLoader/V3Region.h"
#include "Loaders/SuperRegionLoader/V3SuperRegion.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_V3SuperRegionLoaderTests, superRegionLoaderCanLoadFromDisk)
{
	V3::SuperRegionLoader superRegionLoader;

	EXPECT_EQ(0, superRegionLoader.getSuperRegions().size());

	superRegionLoader.loadSuperRegions(modFS);

	EXPECT_EQ(2, superRegionLoader.getSuperRegions().size());

	EXPECT_TRUE(superRegionLoader.getSuperRegions().contains("test_1_strategic_regions"));
	EXPECT_TRUE(superRegionLoader.getSuperRegions().contains("test_2_strategic_regions"));

	const auto& test1sr = superRegionLoader.getSuperRegions().at("test_1_strategic_regions");
	const auto& test2sr = superRegionLoader.getSuperRegions().at("test_2_strategic_regions");

	EXPECT_TRUE(test1sr->containsRegion("region_a"));
	EXPECT_TRUE(test1sr->containsRegion("region_b"));
	EXPECT_TRUE(test2sr->containsRegion("region_c"));

	const auto& region_a = test1sr->getRegions().at("region_a");
	const auto& region_b = test1sr->getRegions().at("region_b");
	const auto& region_c = test2sr->getRegions().at("region_c");

	EXPECT_TRUE(region_a->containsState("STATE_TEST_1"));
	EXPECT_TRUE(region_a->containsState("STATE_TEST_2"));
	EXPECT_TRUE(region_b->containsState("STATE_TEST_3"));
	EXPECT_TRUE(region_c->containsState("STATE_TEST_4"));
}

TEST(V3World_V3SuperRegionLoaderTests, nonTXTfilesAreIgnored)
{
	V3::SuperRegionLoader superRegionLoader;

	EXPECT_EQ(0, superRegionLoader.getSuperRegions().size());

	superRegionLoader.loadSuperRegions(modFS);

	EXPECT_FALSE(superRegionLoader.getSuperRegions().contains("ignored_region"));
}
