#include "History/CountryHistoryDate.h"
#include "gtest/gtest.h"

TEST(EU4World_CountryHistoryDateTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::CountryHistoryDate historyDate(input, std::string());

	ASSERT_TRUE(historyDate.getDynasty().empty());
	ASSERT_TRUE(historyDate.getLeaders().empty());
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
	const EU4::CountryHistoryDate historyDate(input, std::string());
	const auto& leader1 = historyDate.getLeaders()[0];
	const auto& leader2 = historyDate.getLeaders()[1];

	ASSERT_EQ("Boby", leader1.getName());
	ASSERT_EQ("Carol", leader2.getName());
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
	const EU4::CountryHistoryDate historyDate(input, std::string());
	const auto& leader = historyDate.getLeaders()[0];

	ASSERT_EQ("Boby 1st of Bobbypants", leader.getName());
	ASSERT_EQ("Bobbypants", historyDate.getDynasty());
}

TEST(EU4World_CountryHistoryDateTests, heirAndQueenDoNotLoadDynasties)
{
	std::stringstream input;
	input << "queen = {\n";
	input << "\tname = Boby\n";
	input << "\tdynasty = Bobbypants\n";
	input << "\tleader = {\n";
	input << "\t\tname=\"Boby 1st of Bobbypants\"\n";
	input << "\t}\n";
	input << "}\n";
	input << "heir = {\n";
	input << "\tname = Boby\n";
	input << "\tdynasty = Bobbypants\n";
	input << "\tleader = {\n";
	input << "\t\tname=\"Boby 2nd of Bobbypants\"\n";
	input << "\t}\n";
	input << "}\n";
	const EU4::CountryHistoryDate historyDate(input, std::string());
	const auto& leader1 = historyDate.getLeaders()[0];
	const auto& leader2 = historyDate.getLeaders()[1];
	
	ASSERT_EQ("Boby 1st of Bobbypants", leader1.getName());
	ASSERT_EQ("Boby 2nd of Bobbypants", leader2.getName());
	ASSERT_TRUE(historyDate.getDynasty().empty());
}
