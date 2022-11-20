#include "PoliticalManager/CountryDefinitionLoader/Country.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_CountryTests, DefaultsDefaultToDefaults)
{
	const V3::Country country;
	EXPECT_TRUE(country.getTag().empty());
	EXPECT_FALSE(country.getVanillaData());
	EXPECT_TRUE(country.getSubStates().empty());
	EXPECT_FALSE(country.getSourceCountry());
}

TEST(V3World_CountryTests, VanillaDataSpawnsEmptyWhenInitializedFromStream)
{
	std::stringstream input;
	V3::Country country;
	country.initializeCountry(input);

	EXPECT_TRUE(country.getVanillaData()->type.empty());
	EXPECT_TRUE(country.getVanillaData()->tier.empty());
	EXPECT_TRUE(country.getVanillaData()->cultures.empty());
	EXPECT_TRUE(country.getVanillaData()->religion.empty());
	EXPECT_TRUE(country.getVanillaData()->capitalStateName.empty());
	EXPECT_EQ(std::nullopt, country.getVanillaData()->color);
	EXPECT_FALSE(country.getVanillaData()->is_named_from_capital);
}

TEST(V3World_CountryTests, CountryCanBeInitializedViaVanillaData)
{
	std::stringstream input;
	input << "country_type = recognized\n ";
	input << "tier = empire\n ";
	input << "cultures = { cul1 cul2 cul2 }\n ";
	input << "religion = protestant\n ";
	input << "capital = STATE_TEST_1\n ";
	input << "color = {147 130 110}\n ";
	input << "is_named_from_capital = yes\n ";
	V3::Country country;
	country.initializeCountry(input);

	EXPECT_EQ("recognized", country.getVanillaData()->type);
	EXPECT_EQ("empire", country.getVanillaData()->tier);
	EXPECT_EQ(2, country.getVanillaData()->cultures.size());
	EXPECT_THAT(country.getVanillaData()->cultures, testing::UnorderedElementsAre("cul1", "cul2"));
	EXPECT_EQ("protestant", country.getVanillaData()->religion);
	EXPECT_EQ("STATE_TEST_1", country.getVanillaData()->capitalStateName);
	EXPECT_EQ(commonItems::Color(std::array{147, 130, 110}), country.getVanillaData()->color);
	EXPECT_TRUE(country.getVanillaData()->is_named_from_capital);
}

TEST(V3World_CountryTests, CountryTagCanBeSetAndGet)
{
	V3::Country country;

	country.setTag("TAG");

	EXPECT_EQ("TAG", country.getTag());
}
