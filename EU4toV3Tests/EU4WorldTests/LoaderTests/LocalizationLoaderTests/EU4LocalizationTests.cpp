#include "CommonFunctions.h"
#include "LocalizationLoader/EU4LocalizationLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_LocalizationLoaderTests, localizationsReturnsLocMapForKey)
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
	locs.loadLocalizations(input);
	locs.loadLocalizations(input2);
	locs.loadLocalizations(input3);

	EXPECT_THAT(*locs.getTextInEachLanguage("key1"),
		 testing::UnorderedElementsAre(testing::Pair("english", "replaced value 1"), testing::Pair("french", "valuee 1")));
	EXPECT_THAT(*locs.getTextInEachLanguage("key2"),
		 testing::UnorderedElementsAre(testing::Pair("english", "value \"subquoted\" 2"), testing::Pair("french", "valuee \"subquoted\" 2")));
}

TEST(EU4World_LocalizationLoaderTests, localizationsReturnsNulloptMapForMissingKey)
{
	const EU4::EU4LocalizationLoader locs;
	const auto& locMap = locs.getTextInEachLanguage("key1");

	EXPECT_FALSE(locMap);
}
