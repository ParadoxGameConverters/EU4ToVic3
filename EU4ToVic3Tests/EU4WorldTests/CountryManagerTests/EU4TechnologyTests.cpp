#include "CountryManager/EU4Technology.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4TechnologyTests, primitivesDefaultToZero)
{
	std::stringstream input;
	const EU4::EU4Technology technology(input);

	EXPECT_EQ(0, technology.getAdm());
	EXPECT_EQ(0, technology.getDip());
	EXPECT_EQ(0, technology.getMil());
}

TEST(EU4World_EU4TechnologyTests, technologyLevelsCanBeLoaded)
{
	std::stringstream input;
	input << "adm_tech = 1\n";
	input << "dip_tech = 2\n";
	input << "mil_tech = 3\n";
	const EU4::EU4Technology technology(input);

	EXPECT_EQ(1, technology.getAdm());
	EXPECT_EQ(2, technology.getDip());
	EXPECT_EQ(3, technology.getMil());
}
