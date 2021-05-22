#include "Localization/EU4Localization.h"
#include "gtest/gtest.h"
#include "CommonFunctions.h"

TEST(EU4World_EU4LocalizationTests, localizationsCanBeLoadedAndMatched)
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

	EU4::EU4Localization locs;
	locs.readFromStream(input);
	locs.readFromStream(input2);
	locs.readFromStream(input3);
	
	ASSERT_EQ("replaced value 1", *locs.getText("key1", "english"));
	ASSERT_EQ("value \"subquoted\" 2", *locs.getText("key2", "english"));
	ASSERT_EQ("valuee 1", *locs.getText("key1", "french"));
	ASSERT_EQ("valuee \"subquoted\" 2", *locs.getText("key2", "french"));
}

TEST(EU4World_EU4LocalizationTests, localizationsReturnNulloptForMissingKey)
{
	const EU4::EU4Localization locs;

	ASSERT_FALSE(locs.getText("key1", "english"));
}

TEST(EU4World_EU4LocalizationTests, localizationsReturnsNulloptForMissingLanguage)
{
	EU4::EU4Localization locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key1: \"value 1\" # comment\n";
	locs.readFromStream(input);

	ASSERT_FALSE(locs.getText("key1", "french"));
}

TEST(EU4World_EU4LocalizationTests, localizationsReturnsLocMapForKey)
{
	EU4::EU4Localization locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key1: \"value 1\" # comment\n";
	locs.readFromStream(input);
	std::stringstream input2;
	input2 << commonItems::utf8BOM << "l_french:\n";
	input2 << " key1: \"valuee 1\"\n";
	locs.readFromStream(input2);

	auto locMap = locs.getTextInEachLanguage("key1");

	ASSERT_EQ("value 1", (*locMap)["english"]);
	ASSERT_EQ("valuee 1", (*locMap)["french"]);
}

TEST(EU4World_EU4LocalizationTests, localizationsReturnsNulloptMapForMissingKey)
{
	const EU4::EU4Localization locs;
	const auto& locMap = locs.getTextInEachLanguage("key1");

	ASSERT_FALSE(locMap);
}
