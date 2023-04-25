#include "CultureLoader/CultureParser.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::ElementsAre;

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

	EXPECT_THAT(culture.getMaleNames(), ElementsAre("Bob", "Jon"));

	const std::vector<std::string> moreNames = {"Dod", "Kro"};

	culture.addMaleNames(moreNames);

	EXPECT_THAT(culture.getMaleNames(), ElementsAre("Bob", "Jon", "Dod", "Kro"));
}

TEST(EU4World_CultureParserTests, femaleNamesCanBeRetrievedAndAdded)
{
	std::stringstream input;
	input << "female_names = { Bob Jon }";
	EU4::CultureParser culture(input);

	EXPECT_THAT(culture.getFemaleNames(), ElementsAre("Bob", "Jon"));

	const std::vector<std::string> moreNames = {"Dod", "Kro"};

	culture.addFemaleNames(moreNames);

	EXPECT_THAT(culture.getFemaleNames(), ElementsAre("Bob", "Jon", "Dod", "Kro"));
}

TEST(EU4World_CultureParserTests, dynastyNamesCanBeRetrievedAndAdded)
{
	std::stringstream input;
	input << "dynasty_names = { Bob Jon }";
	EU4::CultureParser culture(input);

	EXPECT_THAT(culture.getDynastyNames(), ElementsAre("Bob", "Jon"));

	const std::vector<std::string> moreNames = {"Dod", "Kro"};

	culture.addDynastyNames(moreNames);

	EXPECT_THAT(culture.getDynastyNames(), ElementsAre("Bob", "Jon", "Dod", "Kro"));
}

TEST(EU4World_CultureParserTests, primaryTagCanBeLoadedAndAdded)
{
	std::stringstream input;
	input << "primary = TAG";
	EU4::CultureParser culture(input);

	EXPECT_EQ("TAG", culture.getPrimaryTag());

	culture.setPrimaryTag("GAT");
	EXPECT_EQ("GAT", culture.getPrimaryTag());
}
