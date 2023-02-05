#include "Loaders/PopLoader/PopLoader.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_PopLoaderTests, DefaultsDefaultToDefaults)
{
	const V3::PopLoader popLoader;

	EXPECT_TRUE(popLoader.getStatePops().empty());
}

TEST(V3World_PopLoaderTests, PopsCanBeLoadedFromMultipleFiles)
{
	V3::PopLoader popLoader;
	popLoader.loadPops(modFS);

	ASSERT_EQ(4, popLoader.getStatePops().size());

	const auto& sp1 = popLoader.getStatePops().at("STATE_TEST_LAND1");
	const auto& sp2 = popLoader.getStatePops().at("STATE_TEST_LAND2");
	const auto& sp3 = popLoader.getStatePops().at("STATE_TEST_LAND3");
	const auto& sp4 = popLoader.getStatePops().at("STATE_TEST_LAND4");

	EXPECT_EQ("STATE_TEST_LAND1", sp1.getStateName());
	EXPECT_EQ(600, sp1.getPopCount());

	EXPECT_EQ("STATE_TEST_LAND2", sp2.getStateName());
	EXPECT_EQ(3000, sp2.getPopCount());

	EXPECT_EQ("STATE_TEST_LAND3", sp3.getStateName());
	EXPECT_EQ(900, sp3.getPopCount());

	EXPECT_EQ("STATE_TEST_LAND4", sp4.getStateName());
	EXPECT_EQ(1000, sp4.getPopCount());
}

TEST(V3World_PopLoaderTests, PopLoaderIgnoresNonTXTFiles)
{
	V3::PopLoader popLoader;
	popLoader.loadPops(modFS);

	EXPECT_FALSE(popLoader.getStatePops().contains("STATE_TEST_EXCESS"));
}
