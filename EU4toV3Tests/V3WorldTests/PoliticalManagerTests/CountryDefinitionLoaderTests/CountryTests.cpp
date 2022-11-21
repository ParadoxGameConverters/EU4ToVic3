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

	ASSERT_TRUE(country.getVanillaData());
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

TEST(V3World_CountryTests, CountryCanImportColorFromEU4)
{
	V3::Country country;
	country.setTag("GAT");
	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setTag("TAG");
	srcCountry->setMapColor(commonItems::Color(std::array{1, 2, 3}));
	country.setSourceCountry(srcCountry);

	country.initializeFromEU4Country();

	EXPECT_TRUE(country.getProcessedData().color);
	EXPECT_EQ(commonItems::Color(std::array{1, 2, 3}), country.getProcessedData().color);
}

TEST(V3World_CountryTests, CountryWillOverrideEU4colorWithVic3Color)
{
	std::stringstream input;
	input << "color = { 4 5 6 }";
	V3::Country country;
	country.initializeCountry(input);
	country.setTag("GAT");

	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setTag("TAG");
	srcCountry->setMapColor(commonItems::Color(std::array{1, 2, 3}));
	country.setSourceCountry(srcCountry);

	country.initializeFromEU4Country();

	EXPECT_TRUE(country.getProcessedData().color);
	EXPECT_EQ(commonItems::Color(std::array{4, 5, 6}), country.getProcessedData().color);
}

TEST(V3World_CountryTests, LocalizationsAreAvailable)
{
	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setTag("TAG");
	srcCountry->setLocalizationName("english", "Name1");
	srcCountry->setLocalizationName("albanian", "Name2");
	srcCountry->setLocalizationName("alien", "Name3");
	srcCountry->setLocalizationAdjective("english", "Adj1");
	srcCountry->setLocalizationAdjective("albanian", "Adj2");
	srcCountry->setLocalizationAdjective("alien", "Adj3");

	V3::Country country;
	country.setTag("GAT");
	country.setSourceCountry(srcCountry);
	country.initializeFromEU4Country();

	EXPECT_EQ("Name1", country.getName("english"));
	EXPECT_EQ("Name2", country.getName("albanian"));
	EXPECT_EQ("Name3", country.getName("alien"));
	EXPECT_EQ("Adj1", country.getAdjective("english"));
	EXPECT_EQ("Adj2", country.getAdjective("albanian"));
	EXPECT_EQ("Adj3", country.getAdjective("alien"));
}

TEST(V3World_CountryTests, MissingLocalizationsDefaultToEnglish)
{
	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setTag("TAG");
	srcCountry->setLocalizationName("english", "Name1");
	srcCountry->setLocalizationAdjective("english", "Adj1");

	V3::Country country;
	country.setTag("GAT");
	country.setSourceCountry(srcCountry);
	country.initializeFromEU4Country();

	EXPECT_EQ("Name1", country.getName("english"));
	EXPECT_EQ("Name1", country.getName("albanian"));
	EXPECT_EQ("Name1", country.getName("alien"));
	EXPECT_EQ("Adj1", country.getAdjective("english"));
	EXPECT_EQ("Adj1", country.getAdjective("albanian"));
	EXPECT_EQ("Adj1", country.getAdjective("alien"));
}

TEST(V3World_CountryTests, AbsentLocalizationsDefaultToTag)
{
	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setTag("TAG");

	V3::Country country;
	country.setTag("GAT");
	country.setSourceCountry(srcCountry);
	country.initializeFromEU4Country();

	EXPECT_EQ("GAT", country.getName("english"));
	EXPECT_EQ("GAT_ADJ", country.getAdjective("english"));
}

TEST(V3World_CountryTests, CountryCanSpeedCopyVanillaData)
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

	country.copyVanillaData();

	EXPECT_EQ("recognized", country.getProcessedData().type);
	EXPECT_EQ("empire", country.getProcessedData().tier);
	EXPECT_THAT(country.getProcessedData().cultures, testing::UnorderedElementsAre("cul1", "cul2"));
	EXPECT_EQ("protestant", country.getProcessedData().religion);
	EXPECT_EQ("STATE_TEST_1", country.getProcessedData().capitalStateName);
	EXPECT_EQ(commonItems::Color(std::array{147, 130, 110}), country.getProcessedData().color);
	EXPECT_TRUE(country.getProcessedData().is_named_from_capital);
}

TEST(V3World_CountryTests, CountryWontSpeedCopyMissingVanillaData)
{
	V3::Country country;

	country.copyVanillaData();

	EXPECT_TRUE(country.getProcessedData().type.empty());
	EXPECT_TRUE(country.getProcessedData().tier.empty());
	EXPECT_TRUE(country.getProcessedData().cultures.empty());
	EXPECT_TRUE(country.getProcessedData().religion.empty());
	EXPECT_TRUE(country.getProcessedData().capitalStateName.empty());
	EXPECT_FALSE(country.getProcessedData().color);
	EXPECT_FALSE(country.getProcessedData().is_named_from_capital);
}
