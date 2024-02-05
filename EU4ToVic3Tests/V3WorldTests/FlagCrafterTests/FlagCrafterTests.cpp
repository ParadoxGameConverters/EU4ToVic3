#include "CountryManager/EU4Country.h"
#include "FlagCrafter/FlagCrafter.h"
#include "Mappers/CountryMapper/CountryMapper.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_FlagNameLoaderTests, NoLoadedMatchFails)
{
	V3::FlagCrafter flagCrafter;

	EXPECT_FALSE(flagCrafter.getFlagsForEntity("nonsense"));
}

TEST(V3World_FlagNameLoaderTests, DirectLoadedMatchesCanBeReturnedOnce)
{
	V3::FlagCrafter flagCrafter;
	flagCrafter.loadAvailableFlags("TestFiles/configurables/blankMod/output/common/coat_of_arms/coat_of_arms/", "");

	auto match = flagCrafter.getFlagsForEntity("AAA");

	ASSERT_TRUE(match);
	EXPECT_THAT(*match,
		 testing::UnorderedElementsAre(std::pair(V3::FlagCrafter::FLAGTYPE::Default, "legacy_AAA"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Republic, "legacy_AAA_republic"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Monarchy, "legacy_AAA_monarchy"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Fascist, "legacy_AAA_fascist"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Communist, "legacy_AAA_communist")));

	match = flagCrafter.getFlagsForEntity("AAA");
	ASSERT_FALSE(match);
}

TEST(V3World_FlagNameLoaderTests, FuzzyMatchingWorksAsLongAsThereAreCandidates)
{
	V3::FlagCrafter flagCrafter;
	flagCrafter.loadAvailableFlags("TestFiles/configurables/blankMod/output/common/coat_of_arms/coat_of_arms/", "");

	auto match = flagCrafter.getFlagsForEntity("title");

	ASSERT_TRUE(match);
	EXPECT_THAT(*match,
		 testing::UnorderedElementsAre(std::pair(V3::FlagCrafter::FLAGTYPE::Default, "legacy_e_title"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Republic, "legacy_e_title_republic"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Monarchy, "legacy_e_title_monarchy"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Fascist, "legacy_e_title_fascist"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Communist, "legacy_e_title_communist")));

	match = flagCrafter.getFlagsForEntity("title");
	ASSERT_TRUE(match);
	EXPECT_THAT(*match,
		 testing::UnorderedElementsAre(std::pair(V3::FlagCrafter::FLAGTYPE::Default, "legacy_d_title"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Republic, "legacy_d_title_republic"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Monarchy, "legacy_d_title_monarchy"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Fascist, "legacy_d_title_fascist"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Communist, "legacy_d_title_communist")));

	match = flagCrafter.getFlagsForEntity("title");
	ASSERT_FALSE(match);
}

TEST(V3World_FlagNameLoaderTests, FlagsCanBeAssignedViaFlagCodes)
{
	V3::FlagCrafter flagCrafter;
	flagCrafter.loadAvailableFlags("TestFiles/configurables/blankMod/output/common/coat_of_arms/coat_of_arms/", "");

	mappers::CountryMapper countryMapper;
	std::stringstream countryInput;
	countryInput << "link = {eu4 = TA8 flag_code = legacy_d_title}"; // <- we want TA8 to have "title" flagcode.
	countryMapper.loadMappingRules(countryInput);
	const auto eu4country = std::make_shared<EU4::Country>(); // make TA8.
	eu4country->setTag("TA8");
	const auto v3Tag = countryMapper.assignV3TagToEU4Country(eu4country);
	EXPECT_EQ("X00", v3Tag); // got a tag for V3 country.
	auto country = std::make_shared<V3::Country>();
	country->setTag(v3Tag);

	std::map<std::string, std::shared_ptr<V3::Country>> countries;
	countries.emplace(v3Tag, country);

	// now assign flags.
	flagCrafter.distributeAvailableFlags(countries, countryMapper, commonItems::ModFilesystem());

	// do we have them yet?
	const auto& flags = country->getFlags();

	EXPECT_THAT(flags,
		 testing::UnorderedElementsAre(std::pair(V3::FlagCrafter::FLAGTYPE::Default, "legacy_d_title"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Republic, "legacy_d_title_republic"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Monarchy, "legacy_d_title_monarchy"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Fascist, "legacy_d_title_fascist"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Communist, "legacy_d_title_communist")));
}

TEST(V3World_FlagNameLoaderTests, FlagsCanBeAssignedViaTAGs)
{
	V3::FlagCrafter flagCrafter;
	flagCrafter.loadAvailableFlags("TestFiles/configurables/blankMod/output/common/coat_of_arms/coat_of_arms/", "");

	mappers::CountryMapper countryMapper;
	std::stringstream countryInput;
	countryInput << "link = {eu4 = TA8 vic3 = AAA }"; // <- we want TA8 to map to AAA since we have AAA flags.
	countryMapper.loadMappingRules(countryInput);
	const auto eu4country = std::make_shared<EU4::Country>(); // make TA8.
	eu4country->setTag("TA8");
	const auto v3Tag = countryMapper.assignV3TagToEU4Country(eu4country);
	EXPECT_EQ("AAA", v3Tag); // got a tag for V3 country.
	auto country = std::make_shared<V3::Country>();
	country->setTag(v3Tag);

	std::map<std::string, std::shared_ptr<V3::Country>> countries;
	countries.emplace(v3Tag, country);

	// now assign flags.
	flagCrafter.distributeAvailableFlags(countries, countryMapper, commonItems::ModFilesystem());

	// do we have them yet?
	const auto& flags = country->getFlags();

	EXPECT_THAT(flags,
		 testing::UnorderedElementsAre(std::pair(V3::FlagCrafter::FLAGTYPE::Default, "legacy_AAA"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Republic, "legacy_AAA_republic"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Monarchy, "legacy_AAA_monarchy"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Fascist, "legacy_AAA_fascist"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Communist, "legacy_AAA_communist")));
}

TEST(V3World_FlagNameLoaderTests, FlagsCanBeAssignedViaNameMatch)
{
	V3::FlagCrafter flagCrafter;
	flagCrafter.loadAvailableFlags("TestFiles/configurables/blankMod/output/common/coat_of_arms/coat_of_arms/", "");

	const mappers::CountryMapper countryMapper; // no rules whatsoever.

	auto country = std::make_shared<V3::Country>(); // no eu4 country. not relevant. Get it some name.
	V3::ProcessedData data;
	data.namesByLanguage.emplace("english", "Title"); // this should match e_title in flags.
	country->setProcessedData(data);

	std::map<std::string, std::shared_ptr<V3::Country>> countries;
	countries.emplace("TAG", country);

	// now assign flags.
	flagCrafter.distributeAvailableFlags(countries, countryMapper, commonItems::ModFilesystem());

	// do we have them yet?
	const auto& flags = country->getFlags();

	EXPECT_THAT(flags,
		 testing::UnorderedElementsAre(std::pair(V3::FlagCrafter::FLAGTYPE::Default, "legacy_e_title"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Republic, "legacy_e_title_republic"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Monarchy, "legacy_e_title_monarchy"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Fascist, "legacy_e_title_fascist"),
			  std::pair(V3::FlagCrafter::FLAGTYPE::Communist, "legacy_e_title_communist")));
}

TEST(V3World_FlagNameLoaderTests, FlagsWithVanillaMatchesWillNotBeAssigned)
{
	V3::FlagCrafter flagCrafter;
	flagCrafter.loadAvailableFlags("TestFiles/configurables/blankMod/output/common/coat_of_arms/coat_of_arms/",
		 "TestFiles/vic3installation/game/common/flag_definitions/");

	const mappers::CountryMapper countryMapper; // no rules whatsoever.

	auto country = std::make_shared<V3::Country>(); // no eu4 country. not relevant. Get it some name.
	V3::ProcessedData data;
	data.namesByLanguage.emplace("english", "Title"); // this should match e_title in flags.
	country->setProcessedData(data);

	std::map<std::string, std::shared_ptr<V3::Country>> countries;
	countries.emplace("ABC", country); // ABC has an override in vanilla flag definitions and will not be assigned a flag.

	// now assign flags.
	flagCrafter.distributeAvailableFlags(countries, countryMapper, commonItems::ModFilesystem());

	// do we have them yet?
	const auto& flags = country->getFlags();

	EXPECT_TRUE(flags.empty());
}
