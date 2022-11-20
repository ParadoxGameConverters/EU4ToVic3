#include "CountryMapper/CountryMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_CountryMapperTests, tagIsAlphaDigitDigitWorksAsAdvertised)
{
	EXPECT_TRUE(mappers::CountryMapper::tagIsAlphaDigitDigit("C01"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitDigit("CC1"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitDigit("CCC"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitDigit("0C0"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitDigit("0CC"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitDigit("00C"));
}

TEST(Mappers_CountryMapperTests, tagIsAlphaDigitAlphaNumWorksAsAdvertised)
{
	EXPECT_TRUE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("C01"));
	EXPECT_TRUE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("C0C"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("CC1"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("CCC"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("0C0"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("0CC"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("00C"));
}

TEST(Mappers_CountryMapperTests, rulesCanBeLoadedInOrder)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");

	EXPECT_EQ(10, mapper.getMappingRules().size());
	const auto& rule1 = mapper.getMappingRules()[0];
	EXPECT_EQ("TA1", *rule1.getEU4Tag());
	EXPECT_EQ("GA1", *rule1.getV3Tag());
	const auto& rule10 = mapper.getMappingRules()[9];
	EXPECT_EQ("TA8", *rule10.getEU4Tag());
	EXPECT_EQ("fc8", *rule10.getFlagCode());
}
