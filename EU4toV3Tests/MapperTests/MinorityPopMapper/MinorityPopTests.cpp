#include "MinorityPops/MinorityPop.h"
#include "gtest/gtest.h"

TEST(Mappers_MinorityPopTests, primitivesDefaultToEmpty)
{
	std::stringstream input;
	const mappers::MinorityPop mapper(input);

	ASSERT_TRUE(mapper.getCulture().empty());
	ASSERT_TRUE(mapper.getReligion().empty());
}

TEST(Mappers_MinorityPopTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "culture = ashkenazi religion = jewish";
	const mappers::MinorityPop mapper(input);

	ASSERT_EQ("ashkenazi", mapper.getCulture());
	ASSERT_EQ("jewish", mapper.getReligion());
}
