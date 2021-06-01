#include "CommonFunctions.h"
#include "LocalizationLoader/EU4LocalizationLoader.h"
#include "gtest/gtest.h"

TEST(EU4World_LocalizationLoaderTests, localizationsCanBeLoadedAndMatched)
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
	input3 << " key1: \"replaced value 1\"\n";

	EU4::EU4LocalizationLoader locs;
	locs.loadLocalizations(input);
	locs.loadLocalizations(input2);
	locs.loadLocalizations(input3);

	EXPECT_EQ("replaced value 1", *locs.getText("key1", "english"));
	EXPECT_EQ("value \"subquoted\" 2", *locs.getText("key2", "english"));
	EXPECT_EQ("valuee 1", *locs.getText("key1", "french"));
	EXPECT_EQ("valuee \"subquoted\" 2", *locs.getText("key2", "french"));
}

TEST(EU4World_LocalizationLoaderTests, localizationsReturnNulloptForMissingKey)
{
	const EU4::EU4LocalizationLoader locs;

	EXPECT_FALSE(locs.getText("key1", "english"));
}

TEST(EU4World_LocalizationLoaderTests, localizationsReturnsNulloptForMissingLanguage)
{
	EU4::EU4LocalizationLoader locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key1: \"value 1\" # comment\n";
	locs.loadLocalizations(input);

	EXPECT_FALSE(locs.getText("key1", "french"));
}

TEST(EU4World_LocalizationLoaderTests, localizationsReturnsLocMapForKey)
{
	EU4::EU4LocalizationLoader locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key1: \"value 1\" # comment\n";
	locs.loadLocalizations(input);
	std::stringstream input2;
	input2 << commonItems::utf8BOM << "l_french:\n";
	input2 << " key1: \"valuee 1\"\n";
	locs.loadLocalizations(input2);

	auto locMap = locs.getTextInEachLanguage("key1");

	EXPECT_EQ("value 1", (*locMap)["english"]);
	EXPECT_EQ("valuee 1", (*locMap)["french"]);
}

TEST(EU4World_LocalizationLoaderTests, localizationsReturnsNulloptMapForMissingKey)
{
	const EU4::EU4LocalizationLoader locs;
	const auto& locMap = locs.getTextInEachLanguage("key1");

	EXPECT_FALSE(locMap);
}
