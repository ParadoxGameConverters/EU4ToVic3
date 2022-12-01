#include "CommonFunctions.h"
#include "Loaders/LocLoader/LocalizationLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

V3::LocalizationLoader prepLoader()
{
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key1: \"value 1\" # comment\n";
	input << " key2: \"value \"subquoted\" 2\"\n";
	std::stringstream input2;
	input2 << commonItems::utf8BOM << "l_french:\n";
	input2 << " key1: \"valuee 1\"\n";
	input2 << " key2: \"valuee \"subquoted\" 2\"\n";
	std::stringstream input3;
	input3 << commonItems::utf8BOM << "l_english:\n";
	input3 << " key1: \"replaced value 1\"\n"; // <- modded overrides

	V3::LocalizationLoader locs;
	locs.scrapeStream(input, "english");
	locs.scrapeStream(input2, "french");
	locs.scrapeStream(input3, "english");

	return locs;
}

TEST(V3World_LocalizationLoaderTests, localizationsReturnsLocMapForKey)
{
	const auto& locs = prepLoader();

	EXPECT_THAT(*locs.getLocMapForKey("key1"), testing::UnorderedElementsAre(testing::Pair("english", "replaced value 1"), testing::Pair("french", "valuee 1")));
	EXPECT_THAT(*locs.getLocMapForKey("key2"),
		 testing::UnorderedElementsAre(testing::Pair("english", "value \"subquoted\" 2"), testing::Pair("french", "valuee \"subquoted\" 2")));
}

TEST(V3World_LocalizationLoaderTests, localizationsReturnsLocValueForKey)
{
	const auto& locs = prepLoader();

	EXPECT_EQ("replaced value 1", locs.getLocForKey("key1", "english"));
	EXPECT_EQ("valuee \"subquoted\" 2", locs.getLocForKey("key2", "french"));
}

TEST(V3World_LocalizationLoaderTests, localizationsReturnsEnglishWhenStuck)
{
	const auto& locs = prepLoader();

	EXPECT_EQ("replaced value 1", *locs.getLocForKey("key1", "alien"));
	EXPECT_EQ("value \"subquoted\" 2", *locs.getLocForKey("key2", "alien"));
}

TEST(V3World_LocalizationLoaderTests, localizationsReturnsNulloptMapForMissingKeyMap)
{
	const auto& locs = prepLoader();

	const auto& locMap = locs.getLocMapForKey("key3");

	EXPECT_FALSE(locMap);
}

TEST(V3World_LocalizationLoaderTests, localizationsReturnsNulloptMapForMissingKey)
{
	const auto& locs = prepLoader();

	const auto& value = locs.getLocForKey("key3", "english");

	EXPECT_FALSE(value);
}
