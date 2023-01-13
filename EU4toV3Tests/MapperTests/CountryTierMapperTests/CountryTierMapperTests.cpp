#include "CountryTierMapper/CountryTierMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_CountryTierMapperTests, RanksCanBeMatchedForVanilla)
{
	mappers::CountryTierMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_tiers.txt");

	EXPECT_FALSE(mapper.matchCountryTier(9, 0));
	EXPECT_TRUE(mapper.matchCountryTier(2, 4));
	EXPECT_EQ("kingdom", mapper.matchCountryTier(2, 4));
}

TEST(Mappers_CountryTierMapperTests, RanksCanBeMatchedForVN)
{
	mappers::CountryTierMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_tiers.txt");

	EXPECT_FALSE(mapper.matchCountryTier(9, 0, true));
	EXPECT_TRUE(mapper.matchCountryTier(2, 4, true));
	EXPECT_EQ("grand_principality", *mapper.matchCountryTier(2, 4, true));
}
