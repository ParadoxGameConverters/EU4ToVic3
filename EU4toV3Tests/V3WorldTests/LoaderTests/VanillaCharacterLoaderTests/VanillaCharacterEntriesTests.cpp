#include "Loaders/VanillaCharacterLoader/VanillaCharacterEntries.h"
#include "gtest/gtest.h"

TEST(V3World_VanillaCharacterEntriesTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaCharacterEntries entries;

	EXPECT_TRUE(entries.getCharacterElements().empty());
}

TEST(V3World_VanillaCharacterEntriesTests, entriesCanBeLoaded)
{
	std::stringstream input;
	input << "c:GBR = {\n";
	input << "	create_character = {\n";
	input << "		first_name = \"Khalifa\"\n";
	input << "	}\n";
	input << "}\n";
	input << "c:AUS = {\n";
	input << "	create_character = {\n";
	input << "		first_name = \"al_Qasim\"\n";
	input << "	}\n";
	input << "	create_character = {\n";
	input << "		first_name = \"Said\"\n";
	input << "	}\n";
	input << "}\n";
	input << "c:AUS = {\n";
	input << "	create_character = {\n";
	input << "		first_name = \"Saeed\"\n";
	input << "	}\n";
	input << "}\n";
	const V3::VanillaCharacterEntries entries(input);

	ASSERT_EQ(2, entries.getCharacterElements().size());
	const auto& gbr = entries.getCharacterElements().at("GBR");
	const auto& aus = entries.getCharacterElements().at("AUS");

	EXPECT_EQ("create_character = {\n\t\tfirst_name = \"Khalifa\"\n\t}", gbr[0]);

	EXPECT_EQ("create_character = {\n\t\tfirst_name = \"al_Qasim\"\n\t}", aus[0]);
	EXPECT_EQ("create_character = {\n\t\tfirst_name = \"Said\"\n\t}", aus[1]);
	EXPECT_EQ("create_character = {\n\t\tfirst_name = \"Saeed\"\n\t}", aus[2]);
}
