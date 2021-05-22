#include "Country/EU4CountryFlags.h"
#include "gtest/gtest.h"

TEST(EU4World_Country_EU4CountryFlagsTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::EU4CountryFlags flags(input);

	ASSERT_TRUE(flags.getFlags().empty());
}

TEST(EU4World_Country_EU4CountryFlagsTests, flagsCanBeLoaded)
{
	std::stringstream input;
	input << "flag1 = 11.11.11\n";
	input << "flag2 = color3\n";
	input << "flag3 = {14 34}\n";
	const EU4::EU4CountryFlags flags(input);

	ASSERT_EQ(3, flags.getFlags().size());
	ASSERT_TRUE(flags.getFlags().count("flag1"));
	ASSERT_TRUE(flags.getFlags().count("flag2"));
	ASSERT_TRUE(flags.getFlags().count("flag3"));
}
