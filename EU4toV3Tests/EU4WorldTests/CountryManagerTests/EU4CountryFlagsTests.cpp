#include "CountryManager/EU4CountryFlags.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_EU4CountryFlagsTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::EU4CountryFlags flags(input);

	EXPECT_TRUE(flags.getFlags().empty());
}

TEST(EU4World_EU4CountryFlagsTests, flagsCanBeLoaded)
{
	std::stringstream input;
	input << "flag1 = 11.11.11\n";
	input << "flag2 = color3\n";
	input << "flag3 = {14 34}\n";
	const EU4::EU4CountryFlags flags(input);

	EXPECT_THAT(flags.getFlags(), UnorderedElementsAre("flag1", "flag2", "flag3"));
}
