#include "CommonFunctions.h"
#include "LocalizationLoader/EU4LocalizationLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

EU4::EU4LocalizationLoader prepLoader()
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

	EU4::EU4LocalizationLoader locs;
	locs.loadLocalizations(input3); // mods load first.
	locs.loadLocalizations(input);
	locs.loadLocalizations(input2);

	return locs;
}

TEST(EU4World_LocalizationLoaderTests, localizationsReturnsLocMapForKey)
{
	const auto locs = prepLoader();

	EXPECT_THAT(*locs.getTextInEachLanguage("key1"),
		 testing::UnorderedElementsAre(testing::Pair("english", "replaced value 1"), testing::Pair("french", "valuee 1")));
	EXPECT_THAT(*locs.getTextInEachLanguage("key2"),
		 testing::UnorderedElementsAre(testing::Pair("english", "value \"subquoted\" 2"), testing::Pair("french", "valuee \"subquoted\" 2")));
}

TEST(EU4World_LocalizationLoaderTests, localizationsReturnsLocValueForKey)
{
	const auto locs = prepLoader();

	EXPECT_EQ("replaced value 1", locs.getTextForKey("key1", "english"));
	EXPECT_EQ("valuee \"subquoted\" 2", locs.getTextForKey("key2", "french"));
}

TEST(EU4World_LocalizationLoaderTests, localizationsReturnsEnglishWhenStuck)
{
	const auto locs = prepLoader();

	EXPECT_EQ("replaced value 1", *locs.getTextForKey("key1", "alien"));
	EXPECT_EQ("value \"subquoted\" 2", *locs.getTextForKey("key2", "alien"));
}

TEST(EU4World_LocalizationLoaderTests, localizationsReturnsNulloptMapForMissingKeyMap)
{
	const auto locs = prepLoader();

	const auto& locMap = locs.getTextInEachLanguage("key3");

	EXPECT_FALSE(locMap);
}

TEST(EU4World_LocalizationLoaderTests, localizationsReturnsNulloptMapForMissingKey)
{
	const auto locs = prepLoader();

	const auto& value = locs.getTextForKey("key3", "english");

	EXPECT_FALSE(value);
}
