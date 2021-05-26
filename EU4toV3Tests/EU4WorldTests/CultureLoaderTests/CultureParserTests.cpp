#include "CultureLoader/CultureParser.h"
#include "gtest/gtest.h"

TEST(EU4World_CultureParserTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const EU4::CultureParser culture(input);

	EXPECT_TRUE(culture.getMaleNames().empty());
	EXPECT_TRUE(culture.getFemaleNames().empty());
	EXPECT_TRUE(culture.getDynastyNames().empty());
	EXPECT_TRUE(culture.getPrimaryTag().empty());
}

TEST(EU4World_CultureParserTests, maleNamesCanBeRetrievedAndAdded)
{
	std::stringstream input;
	input << "male_names = { Bob Jon }";
	EU4::CultureParser culture(input);

	EXPECT_EQ(2, culture.getMaleNames().size());
	EXPECT_EQ("Bob", culture.getMaleNames()[0]);
	EXPECT_EQ("Jon", culture.getMaleNames()[1]);

	const std::vector<std::string> moreNames = {"Dod", "Kro"};

	culture.addMaleNames(moreNames);

	EXPECT_EQ(4, culture.getMaleNames().size());
	EXPECT_EQ("Dod", culture.getMaleNames()[2]);
	EXPECT_EQ("Kro", culture.getMaleNames()[3]);
}

TEST(EU4World_CultureParserTests, femaleNamesCanBeRetrievedAndAdded)
{
	std::stringstream input;
	input << "female_names = { Bob Jon }";
	EU4::CultureParser culture(input);

	EXPECT_EQ(2, culture.getFemaleNames().size());
	EXPECT_EQ("Bob", culture.getFemaleNames()[0]);
	EXPECT_EQ("Jon", culture.getFemaleNames()[1]);

	const std::vector<std::string> moreNames = {"Dod", "Kro"};

	culture.addFemaleNames(moreNames);

	EXPECT_EQ(4, culture.getFemaleNames().size());
	EXPECT_EQ("Dod", culture.getFemaleNames()[2]);
	EXPECT_EQ("Kro", culture.getFemaleNames()[3]);
}

TEST(EU4World_CultureParserTests, dynastyNamesCanBeRetrievedAndAdded)
{
	std::stringstream input;
	input << "dynasty_names = { Bob Jon }";
	EU4::CultureParser culture(input);

	EXPECT_EQ(2, culture.getDynastyNames().size());
	EXPECT_EQ("Bob", culture.getDynastyNames()[0]);
	EXPECT_EQ("Jon", culture.getDynastyNames()[1]);

	const std::vector<std::string> moreNames = {"Dod", "Kro"};

	culture.addDynastyNames(moreNames);

	EXPECT_EQ(4, culture.getDynastyNames().size());
	EXPECT_EQ("Dod", culture.getDynastyNames()[2]);
	EXPECT_EQ("Kro", culture.getDynastyNames()[3]);
}

TEST(Mappers_CultureTests, primaryTagCanBeLoadedAndAdded)
{
	std::stringstream input;
	input << "primary = TAG";
	EU4::CultureParser culture(input);

	EXPECT_EQ("TAG", culture.getPrimaryTag());

	culture.setPrimaryTag("GAT");
	EXPECT_EQ("GAT", culture.getPrimaryTag());
}

