#include "CountryTierMapper/CountryTierMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_CountryTierMappingTests, NoMatchReturnNull)
{
	const mappers::CountryTierMapping mapping;

	EXPECT_FALSE(mapping.matchTier(1, 1));
}

TEST(Mappers_CountryTierMappingTests, MatchSucceedsOnRank)
{
	std::stringstream input;
	input << "rank = 4";
	const mappers::CountryTierMapping mapping(input, "theRank");

	EXPECT_FALSE(mapping.matchTier(5, 0));

	ASSERT_TRUE(mapping.matchTier(4, 0));
	EXPECT_EQ("theRank", *mapping.matchTier(4, 0));
}

TEST(Mappers_CountryTierMappingTests, MatchSucceedsOnRankAndSize)
{
	std::stringstream input;
	input << "rank = 4 size = 5";
	const mappers::CountryTierMapping mapping(input, "theRank");

	EXPECT_FALSE(mapping.matchTier(4, 10));

	ASSERT_TRUE(mapping.matchTier(4, 5));
	ASSERT_TRUE(mapping.matchTier(4, 2));
	EXPECT_EQ("theRank", *mapping.matchTier(4, 5));
}
