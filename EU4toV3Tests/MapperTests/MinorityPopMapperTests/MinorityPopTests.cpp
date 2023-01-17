#include "MinorityPopMapper/MinorityPop.h"
#include "gtest/gtest.h"

TEST(Mappers_MinorityPopTests, primitivesDefaultToEmpty)
{
	std::stringstream input;
	const mappers::MinorityPop mapper(input);

	EXPECT_TRUE(mapper.getCulture().empty());
	EXPECT_TRUE(mapper.getReligion().empty());
}

TEST(Mappers_MinorityPopTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "culture = ashkenazi religion = jewish";
	const mappers::MinorityPop mapper(input);

	EXPECT_EQ("ashkenazi", mapper.getCulture());
	EXPECT_EQ("jewish", mapper.getReligion());
}
