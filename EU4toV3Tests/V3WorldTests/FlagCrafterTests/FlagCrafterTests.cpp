#include "FlagCrafter/FlagCrafter.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_FlagNameLoaderTests, NoLoadedMatchFails)
{
	V3::FlagCrafter flagCrafter;

	EXPECT_FALSE(flagCrafter.getFlagsForEntity("nonsense"));
}

TEST(V3World_FlagNameLoaderTests, DirectLoadedMatchesCanBeReturnedOnce)
{
	V3::FlagCrafter flagCrafter;
	flagCrafter.loadAvailableFlags("TestFiles/configurables/blankMod/output/common/coat_of_arms/coat_of_arms/");

	auto match = flagCrafter.getFlagsForEntity("AAA");

	ASSERT_TRUE(match);
	EXPECT_THAT(*match,
		 testing::UnorderedElementsAre(std::pair(V3::FlagCrafter::FLAGTYPE::Default, "legacy_AAA"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Republic, "legacy_AAA_republic"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Monarchy, "legacy_AAA_monarchy"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Fascist, "legacy_AAA_fascist"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Communist, "legacy_AAA_communist")));

	match = flagCrafter.getFlagsForEntity("AAA");
	ASSERT_FALSE(match);
}

TEST(V3World_FlagNameLoaderTests, FuzzyMatchingWorksAsLongAsThereAreCandidates)
{
	V3::FlagCrafter flagCrafter;
	flagCrafter.loadAvailableFlags("TestFiles/configurables/blankMod/output/common/coat_of_arms/coat_of_arms/");

	auto match = flagCrafter.getFlagsForEntity("title");

	ASSERT_TRUE(match);
	EXPECT_THAT(*match,
		 testing::UnorderedElementsAre(std::pair(V3::FlagCrafter::FLAGTYPE::Default, "legacy_e_title"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Republic, "legacy_e_title_republic"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Monarchy, "legacy_e_title_monarchy"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Fascist, "legacy_e_title_fascist"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Communist, "legacy_e_title_communist")));

	match = flagCrafter.getFlagsForEntity("title");
	ASSERT_TRUE(match);
	EXPECT_THAT(*match,
		 testing::UnorderedElementsAre(std::pair(V3::FlagCrafter::FLAGTYPE::Default, "legacy_d_title"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Republic, "legacy_d_title_republic"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Monarchy, "legacy_d_title_monarchy"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Fascist, "legacy_d_title_fascist"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Communist, "legacy_d_title_communist")));

	match = flagCrafter.getFlagsForEntity("title");
	ASSERT_FALSE(match);
}
