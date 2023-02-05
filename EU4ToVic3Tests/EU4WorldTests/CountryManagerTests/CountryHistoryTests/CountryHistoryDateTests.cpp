#include "CountryManager/CountryHistory/CountryHistoryDate.h"
#include "gtest/gtest.h"

TEST(EU4World_CountryHistoryDateTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::CountryHistoryDate historyDate(input);

	EXPECT_TRUE(historyDate.getCharacters().empty());
}

TEST(EU4World_CountryHistoryDateTests, simpleLeadersCanBeLoaded)
{
	std::stringstream input;
	input << "leader = {\n";
	input << "\tname=Boby\n";
	input << "}\n";
	input << "leader = {\n";
	input << "\tname=Carol\n";
	input << "}\n";
	const EU4::CountryHistoryDate historyDate(input);

	ASSERT_EQ(2, historyDate.getCharacters().size());
	const auto& leader1 = historyDate.getCharacters()[0];
	const auto& leader2 = historyDate.getCharacters()[1];

	EXPECT_EQ("Boby", leader1.leaderName);
	EXPECT_EQ("Carol", leader2.leaderName);
}

TEST(EU4World_CountryHistoryDateTests, monarchLeaderAndDynastyCanBeLoaded)
{
	std::stringstream input;
	input << "monarch = {\n";
	input << "\tname = Boby\n";
	input << "\tdynasty = Bobbypants\n";
	input << "\tleader = {\n";
	input << "\t\tname=\"Boby 1st of Bobbypants\"\n";
	input << "\t}\n";
	input << "}\n";
	const EU4::CountryHistoryDate historyDate(input);

	ASSERT_EQ(1, historyDate.getCharacters().size());
	const auto& leader = historyDate.getCharacters()[0];

	EXPECT_EQ("Boby", leader.name);
	EXPECT_EQ("Bobbypants", leader.dynasty);
	EXPECT_EQ("Boby 1st of Bobbypants", leader.leaderName);
	EXPECT_TRUE(leader.ruler);
}

TEST(EU4World_CountryHistoryDateTests, heirAndQueenCanBeLoaded)
{
	std::stringstream input;
	input << "queen = {\n";
	input << "\tname = Boby\n";
	input << "\tdynasty = Bobbypants\n";
	input << "\tleader = {\n";
	input << "\t\tname=\"Queen 1st of Bobbypants\"\n";
	input << "\t}\n";
	input << "}\n";
	input << "heir = {\n";
	input << "\tname = Boby\n";
	input << "\tdynasty = Bobbypants\n";
	input << "\tleader = {\n";
	input << "\t\tname=\"Heir 2nd of Bobbypants\"\n";
	input << "\t}\n";
	input << "}\n";
	const EU4::CountryHistoryDate historyDate(input);

	ASSERT_EQ(2, historyDate.getCharacters().size());
	const auto& leader1 = historyDate.getCharacters()[0];
	const auto& leader2 = historyDate.getCharacters()[1];

	EXPECT_EQ("Queen 1st of Bobbypants", leader1.leaderName);
	EXPECT_EQ("Bobbypants", leader1.dynasty);
	EXPECT_TRUE(leader1.consort);
	EXPECT_EQ("Heir 2nd of Bobbypants", leader2.leaderName);
	EXPECT_EQ("Bobbypants", leader2.dynasty);
	EXPECT_TRUE(leader2.heir);
}
