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

TEST(V3World_PoliticalManagerTests, PoliticalManagerCanImportVanillaCountries)
{
	const auto countryMapper = std::make_shared<mappers::CountryMapper>();
	countryMapper->loadMappingRules("TestFiles/configurables/country_mappings.txt");

	std::stringstream countryStream;
	const auto ta2 = std::make_shared<EU4::Country>("TA2", countryStream);
	const auto ta3 = std::make_shared<EU4::Country>("TA3", countryStream);
	const auto ta9 = std::make_shared<EU4::Country>("TA9", countryStream);
	const std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA2", ta2}, {"TA3", ta3}, {"TA9", ta9}};

	V3::PoliticalManager politicalManager;

	EXPECT_EQ(0, politicalManager.getCountries().size());
	politicalManager.initializeVanillaCountries("TestFiles/vic3installation/game/");
	EXPECT_EQ(3, politicalManager.getCountries().size()); // we have 3 vanilla vic3 countries, unrelated to eu4.
	politicalManager.loadCountryMapper(countryMapper);
	politicalManager.importEU4Countries(countries);
	EXPECT_EQ(6, politicalManager.getCountries().size()); // we append the 3 imported eu4 countries.

	const auto& country1 = politicalManager.getCountries().at("GA2");
	const auto& country2 = politicalManager.getCountries().at("X00");
	const auto& country3 = politicalManager.getCountries().at("GA9");
	const auto& country4 = politicalManager.getCountries().at("TAG");
	const auto& country5 = politicalManager.getCountries().at("GAT");
	const auto& country6 = politicalManager.getCountries().at("TGA");

	EXPECT_FALSE(country1->getProcessedData().color); // these 3 eu4 countries have no color.
	EXPECT_FALSE(country2->getProcessedData().color);
	EXPECT_FALSE(country3->getProcessedData().color);
	EXPECT_FALSE(country4->getProcessedData().color); // these 3 vanilla ones are also blank.
	EXPECT_FALSE(country5->getProcessedData().color);
	EXPECT_FALSE(country6->getProcessedData().color);

	politicalManager.importVanillaCountries(); // now we process only the 3 vanilla countries.

	EXPECT_FALSE(country1->getProcessedData().color); // these 3 eu4 countries still have no color.
	EXPECT_FALSE(country2->getProcessedData().color);
	EXPECT_FALSE(country3->getProcessedData().color);
	EXPECT_EQ(commonItems::Color(std::array{147, 130, 110}), country4->getProcessedData().color); // however these 3 vanilla ones have their color copied over.
	EXPECT_EQ(commonItems::Color(std::array{0.99f, 0.7f, 0.9f}), country5->getProcessedData().color); // since they are processed standalone.
	EXPECT_EQ(commonItems::Color(std::array{62, 122, 189}), country6->getProcessedData().color);
}
