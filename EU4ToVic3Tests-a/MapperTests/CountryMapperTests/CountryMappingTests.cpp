#include "CountryMapper/CountryMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_CountryMappingTests, DefaultsDefaultToDefaults)
{
	const mappers::CountryMapping countryMapping;

	EXPECT_FALSE(countryMapping.getEU4Tag());
	EXPECT_FALSE(countryMapping.getV3Tag());
	EXPECT_FALSE(countryMapping.getName());
	EXPECT_FALSE(countryMapping.getFlagCode());
	EXPECT_TRUE(countryMapping.getFlags().empty());
	EXPECT_TRUE(countryMapping.getReforms().empty());
}

TEST(Mappers_CountryMappingTests, RulesCanBeLoaded)
{
	std::stringstream input;
	input << "eu4 = TAG vic3 = GAT name = \"The Name\" flag_code = flagCode flag = flag1 flag = flag2 flag = flag2 reform = ref1 reform = ref2 reform = ref2";
	const mappers::CountryMapping countryMapping(input);

	EXPECT_EQ("TAG", *countryMapping.getEU4Tag());
	EXPECT_EQ("GAT", *countryMapping.getV3Tag());
	EXPECT_EQ("The Name", *countryMapping.getName());
	EXPECT_EQ("flagCode", *countryMapping.getFlagCode());
	EXPECT_EQ(2, countryMapping.getFlags().size());
	EXPECT_THAT(countryMapping.getFlags(), testing::UnorderedElementsAre("flag1", "flag2"));
	EXPECT_EQ(2, countryMapping.getReforms().size());
	EXPECT_THAT(countryMapping.getReforms(), testing::UnorderedElementsAre("ref1", "ref2"));
}
