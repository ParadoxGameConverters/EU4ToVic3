#include "PoliticalManager/PoliticalManager.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_PoliticalManagerTests, PoliticalManagerCanInitializeVanillaCountries)
{
	V3::PoliticalManager politicalManager;

	EXPECT_EQ(0, politicalManager.getCountries().size());

	politicalManager.initializeVanillaCountries("TestFiles/vic3installation/game/");

	EXPECT_EQ(3, politicalManager.getCountries().size());

	const auto& country1 = politicalManager.getCountries().at("TAG");
	const auto& country2 = politicalManager.getCountries().at("GAT");
	const auto& country3 = politicalManager.getCountries().at("TGA");

	EXPECT_EQ("TAG", country1->getTag());
	EXPECT_EQ("GAT", country2->getTag());
	EXPECT_EQ("TGA", country3->getTag());
}

TEST(V3World_PoliticalManagerTests, PoliticalManagerCanImportCountries)
{
	const auto countryMapper = std::make_shared<mappers::CountryMapper>();
	countryMapper->loadMappingRules("TestFiles/configurables/country_mappings.txt");

	std::stringstream countryStream;
	const auto ta2 = std::make_shared<EU4::Country>("TA2", countryStream);
	const auto ta3 = std::make_shared<EU4::Country>("TA3", countryStream);
	const auto ta9 = std::make_shared<EU4::Country>("TA9", countryStream);
	const std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA2", ta2}, {"TA3", ta3}, {"TA9", ta9}};

	V3::PoliticalManager politicalManager;
	politicalManager.loadCountryMapper(countryMapper);
	politicalManager.importEU4Countries(countries);

	EXPECT_EQ(3, politicalManager.getCountries().size());

	const auto& country1 = politicalManager.getCountries().at("GA2");
	const auto& country2 = politicalManager.getCountries().at("X00");
	const auto& country3 = politicalManager.getCountries().at("GA9");

	EXPECT_EQ("TA2", country1->getSourceCountry()->getTag());
	EXPECT_EQ("TA3", country2->getSourceCountry()->getTag());
	EXPECT_EQ("TA9", country3->getSourceCountry()->getTag());
}
