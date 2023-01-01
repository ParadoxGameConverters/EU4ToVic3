#include "FlagCrafter/FlagCrafter.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_FlagNameLoaderTests, NoLoadedMatchFails)
{
	const V3::FlagCrafter flagCrafter;

	EXPECT_FALSE(flagCrafter.getFlagsForEntity("nonsense"));
}

TEST(V3World_FlagNameLoaderTests, LoadedMatchesCanBeReturned)
{
	V3::FlagCrafter flagCrafter;
	flagCrafter.loadAvailableFlags("TestFiles/configurables/blankMod/output/common/coat_of_arms/coat_of_arms/");

	const auto& match = flagCrafter.getFlagsForEntity("AAC");

	ASSERT_TRUE(match);
	EXPECT_THAT(*match,
		 testing::UnorderedElementsAre(std::pair(V3::FlagCrafter::FLAGTYPE::Default, "legacy_AAC"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Republic, "legacy_AAC_republic"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Monarchy, "legacy_AAC_monarchy"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Fascist, "legacy_AAC_fascist"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Communist, "legacy_AAC_communist")));
}
