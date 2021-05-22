#include "History/CountryHistory.h"
#include "gtest/gtest.h"

TEST(EU4World_CountryHistoryTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::CountryHistory history(input);

	ASSERT_TRUE(history.getDynasty().empty());
	ASSERT_TRUE(history.getReligion().empty());
	ASSERT_TRUE(history.getPrimaryCulture().empty());
	ASSERT_TRUE(history.getLeaders().empty());
}

TEST(EU4World_CountryHistoryTests, cultureAndReligionCanBeLoaded)
{
	std::stringstream input;
	input << "religion = atheist\n";
	input << "primary_culture = cosmopolitan\n";
	const EU4::CountryHistory history(input);

	ASSERT_EQ("atheist", history.getReligion());
	ASSERT_EQ("cosmopolitan", history.getPrimaryCulture());
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
	const auto& leader1 = history.getLeaders()[0];
	const auto& leader2 = history.getLeaders()[1];
	const auto& leader3 = history.getLeaders()[2];

	ASSERT_EQ(3, history.getLeaders().size());
	ASSERT_EQ("TheGeneral", leader1.getName());
	ASSERT_EQ("TheAdmiral", leader2.getName());
	ASSERT_EQ("TheExplorer", leader3.getName());
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
	const auto& leader1 = history.getLeaders()[0];
	const auto& leader2 = history.getLeaders()[1];
	const auto& leader3 = history.getLeaders()[2];
	
	ASSERT_EQ("Boby 1st of Bobbypants", leader1.getName());
	ASSERT_EQ("Boby 2nd of Bobbypants", leader2.getName());
	ASSERT_EQ("Boby 3rd of Bobbypants", leader3.getName());
	ASSERT_EQ("Bobbypants", history.getDynasty());
}
