#include "Loaders/VanillaStateLoader/VanillaStateLoader.h"
#include "gtest/gtest.h"

TEST(V3World_VanillaStateLoaderTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaStateLoader loader;

	EXPECT_TRUE(loader.getStates().empty());
}

TEST(V3World_VanillaStateLoaderTests, VanillaStatesCanBeLoaded)
{
	V3::VanillaStateLoader loader;
	loader.loadVanillaStates("TestFiles/vic3installation/game/");

	ASSERT_EQ(4, loader.getStates().size());
	EXPECT_TRUE(loader.getStates().contains("STATE_TEST_LAND1"));
	EXPECT_TRUE(loader.getStates().contains("STATE_TEST_LAND2"));
	EXPECT_TRUE(loader.getStates().contains("STATE_TEST_LAND3"));
	EXPECT_TRUE(loader.getStates().contains("STATE_TEST_LAND4"));
}
