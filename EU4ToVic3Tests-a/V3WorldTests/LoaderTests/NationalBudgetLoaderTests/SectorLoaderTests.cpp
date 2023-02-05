#include "CountryManager/EU4Country.h"
#include "Loaders/NationalBudgetLoader/SectorLoader.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_SectorLoaderTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;

	V3::SectorLoader sectorBlueprint;
	sectorBlueprint.loadSector(input);

	V3::Country country;
	country.setSourceCountry(std::make_shared<EU4::Country>());

	EXPECT_DOUBLE_EQ(0, sectorBlueprint.calculateWeight(country));
	EXPECT_TRUE(sectorBlueprint.getBuildings().empty());
}

TEST(V3World_SectorLoaderTests, SectorLoaderCanLoadSector)
{
	std::stringstream input;

	input << "weight = 12\n";
	input << "building_barracks building_fort\n";
	input << "multiply = { value = 0.1 vic3_law = law_quality }\n";
	input << "multiply = { value = 1.5 vic3_law = law_peasant_levy }\n";
	input << "add = { value = 1 is_colony = no }\n";

	V3::SectorLoader sectorBlueprint;
	sectorBlueprint.loadSector(input);

	V3::Country country;
	country.setSourceCountry(std::make_shared<EU4::Country>());
	V3::ProcessedData data;
	data.laws.emplace("law_quality");
	data.laws.emplace("law_peasant_levy");
	country.setProcessedData(data);


	EXPECT_DOUBLE_EQ(7.8, sectorBlueprint.calculateWeight(country));
	EXPECT_THAT(sectorBlueprint.getBuildings(), testing::UnorderedElementsAre("building_barracks", "building_fort"));
}