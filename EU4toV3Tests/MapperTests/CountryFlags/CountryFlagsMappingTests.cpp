#include "CountryFlags/CountryFlagsMapping.h"
#include "gtest/gtest.h"

TEST(Mappers_CountryFlagsMappingTests, SourceFlagDefaultsToBlank)
{
	std::stringstream input;
	const mappers::CountryFlagsMapping countryFlagsMapping(input);

	ASSERT_TRUE(countryFlagsMapping.getEU4Flag().empty());
}

TEST(Mappers_CountryFlagsMappingTests, SourceFlagCanBeSet)
{
	std::stringstream input;
	input << "eu4 = changed_from_colonial_nation";
	const mappers::CountryFlagsMapping countryFlagsMapping(input);

	ASSERT_EQ(countryFlagsMapping.getEU4Flag(), "changed_from_colonial_nation");
}

TEST(Mappers_CountryFlagsMappingTests, TargetFlagsDefaultToBlank)
{
	std::stringstream input;
	const mappers::CountryFlagsMapping countryFlagsMapping(input);

	ASSERT_TRUE(countryFlagsMapping.getV2Flags().empty());
}

TEST(Mappers_CountryFlagsMappingTests, TargetFlagsCanBeSet)
{
	std::stringstream input;
	input << "v2 = post_colonial_country v2 = postcol_1";
	const mappers::CountryFlagsMapping countryFlagsMapping(input);

	ASSERT_TRUE(countryFlagsMapping.getV2Flags().contains("post_colonial_country"));
	ASSERT_TRUE(countryFlagsMapping.getV2Flags().contains("postcol_1"));
}
