#include "Loaders/MajorFormablesLoader/MajorFormablesLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_MajorFormablesLoaderTests, DefaultsDefaultToDefaults)
{
	const V3::MajorFormablesLoader loader;

	EXPECT_TRUE(loader.getMajorFormables().empty());
}

TEST(V3World_MajorFormablesLoaderTests, FormablesCanBeLoaded)
{
	V3::MajorFormablesLoader loader;
	loader.loadMajorFormables("TestFiles/vic3installation/game/common/country_formation/00_major_formables.txt");

	auto formables = loader.getMajorFormables();

	ASSERT_EQ(3, formables.size());
	EXPECT_EQ(6, formables["GER"].getStanzas().size());
	EXPECT_EQ("nationalism", formables["GER"].getRequiredTechnology());
	EXPECT_THAT(formables["GER"].getCultures(), testing::UnorderedElementsAre("north_german", "south_german", "weder_german"));
}
