#include "CountryManager/CountryHistory/CountryHistory.h"
#include "gtest/gtest.h"

TEST(EU4World_CountryHistoryTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::CountryHistory history(input);

	EXPECT_TRUE(history.getReligion().empty());
	EXPECT_TRUE(history.getPrimaryCulture().empty());
}

TEST(EU4World_CountryHistoryTests, cultureAndReligionCanBeLoaded)
{
	std::stringstream input;
	input << "religion = atheist\n";
	input << "primary_culture = cosmopolitan\n";
	const EU4::CountryHistory history(input);

	EXPECT_EQ("atheist", history.getReligion());
	EXPECT_EQ("cosmopolitan", history.getPrimaryCulture());
}

TEST(EU4World_CountryHistoryTests, datesCanBeParsedForLeaders)
{
	std::stringstream input;
	input << "2.2.2 = {\n";
	input << "\tleader = {\n";
	input << "\t\tname = TheGeneral\n";
	input << "\t}";
	input << "}";
	input << "4.4.4 = {\n";
	input << "\tleader = {\n";
	input << "\t\tname = TheAdmiral\n";
	input << "\t}";
	input << "\tleader = {\n";
	input << "\t\tname = TheExplorer\n";
	input << "\t}";
	input << "}";

	const EU4::CountryHistory history(input);
	const auto& leader1 = history.getCharacters()[0];
	const auto& leader2 = history.getCharacters()[1];
	const auto& leader3 = history.getCharacters()[2];

	EXPECT_EQ(3, history.getCharacters().size());
	EXPECT_EQ("TheGeneral", leader1.leaderName);
	EXPECT_EQ("TheAdmiral", leader2.leaderName);
	EXPECT_EQ("TheExplorer", leader3.leaderName);
}

TEST(EU4World_CountryHistoryTests, datesCanBeParsedForRulerLeaders)
{
	std::stringstream input;
	input << "2.2.2 = {\n";
	input << "\tmonarch = {\n";
	input << "\t\tname = Boby\n";
	input << "\t\tdynasty = Bobbypants\n";
	input << "\t\tleader = {\n";
	input << "\t\t\tname=\"Boby 1st of Bobbypants\"\n";
	input << "\t\t}\n";
	input << "\t}\n";
	input << "}\n";

	input << "3.3.3 = {\n";
	input << "\tqueen = {\n";
	input << "\t\tname = Boby\n";
	input << "\t\tdynasty = Bobbypantaloon\n";
	input << "\t\tleader = {\n";
	input << "\t\t\tname=\"Boby 2nd of Bobbypants\"\n";
	input << "\t\t}\n";
	input << "\t}\n";
	input << "\their = {\n";
	input << "\t\tname = Boby\n";
	input << "\t\tdynasty = Bobbypantaloon\n";
	input << "\t\tleader = {\n";
	input << "\t\t\tname=\"Boby 3rd of Bobbypants\"\n";
	input << "\t\t}\n";
	input << "\t}\n";
	input << "}\n";

	const EU4::CountryHistory history(input);
	const auto& leader1 = history.getCharacters()[0];
	const auto& leader2 = history.getCharacters()[1];
	const auto& leader3 = history.getCharacters()[2];

	EXPECT_EQ("Boby 1st of Bobbypants", leader1.leaderName);
	EXPECT_EQ("Boby 2nd of Bobbypants", leader2.leaderName);
	EXPECT_EQ("Boby 3rd of Bobbypants", leader3.leaderName);
}
