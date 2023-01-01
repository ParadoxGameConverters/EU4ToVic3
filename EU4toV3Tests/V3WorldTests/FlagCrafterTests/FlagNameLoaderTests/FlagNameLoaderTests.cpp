#include "FlagCrafter/FlagNameLoader/FlagNameLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_FlagNameLoaderTests, DefaultsDefaultToDefaults)
{
	const V3::FlagNameLoader flagNameLoader;

	EXPECT_TRUE(flagNameLoader.getKnownFlags().empty());
}

TEST(V3World_FlagNameLoaderTests, flagNamesCanBeLoaded)
{
	V3::FlagNameLoader flagNameLoader;
	flagNameLoader.loadKnownFlags("TestFiles/configurables/blankMod/output/common/coat_of_arms/coat_of_arms/");

	EXPECT_THAT(flagNameLoader.getKnownFlags(),
		 testing::UnorderedElementsAre("legacy_AAA",
			  "legacy_AAA_communist",
			  "legacy_AAA_fascist",
			  "legacy_AAA_monarchy",
			  "legacy_AAA_republic",
			  "legacy_AAC",
			  "legacy_AAC_communist",
			  "legacy_AAC_fascist",
			  "legacy_AAC_monarchy",
			  "legacy_AAC_republic",
			  "legacy_title",
			  "legacy_title_communist",
			  "legacy_title_fascist",
			  "legacy_title_monarchy",
			  "legacy_title_republic"));
}
