#include "Loaders/VanillaDiplomaticPlayLoader/VanillaDiplomaticPlayEntries.h"
#include "gtest/gtest.h"

TEST(V3World_VanillaDiplomaticPlayEntriesTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaDiplomaticPlayEntries entries;

	EXPECT_TRUE(entries.getDiploPlayElements().empty());
}

TEST(V3World_VanillaDiplomaticPlayEntriesTests, entriesCanBeLoaded)
{
	std::stringstream input;
	input << "c:GBR = {\n";
	input << "	create_diplomatic_play = {\n";
	input << "		name = ragamuffin_war\n";
	input << "	}\n";
	input << "}\n";
	input << "c:AUS = {\n";
	input << "	create_diplomatic_play = {\n";
	input << "		name = cabanagem_war\n";
	input << "	}\n";
	input << "	create_diplomatic_play = {\n";
	input << "		name = cabanagem_war2\n";
	input << "	}\n";
	input << "}\n";
	input << "c:AUS = {\n";
	input << "	create_diplomatic_play = {\n";
	input << "		name = cabanagem_war3\n";
	input << "	}\n";
	input << "}\n";
	const V3::VanillaDiplomaticPlayEntries entries(input);

	ASSERT_EQ(2, entries.getDiploPlayElements().size());
	const auto& gbr = entries.getDiploPlayElements().at("GBR");
	const auto& aus = entries.getDiploPlayElements().at("AUS");

	EXPECT_EQ("create_diplomatic_play = {\n\t\tname = ragamuffin_war\n\t}", gbr[0]);

	EXPECT_EQ("create_diplomatic_play = {\n\t\tname = cabanagem_war\n\t}", aus[0]);
	EXPECT_EQ("create_diplomatic_play = {\n\t\tname = cabanagem_war2\n\t}", aus[1]);
	EXPECT_EQ("create_diplomatic_play = {\n\t\tname = cabanagem_war3\n\t}", aus[2]);
}
