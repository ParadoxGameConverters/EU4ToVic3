#include "CountryManager/EU4Country.h"
#include "EconomyManager/NationalBudget/Sector.h"
#include "Loaders/NationalBudgetLoader/SectorLoader.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>


TEST(V3World_SectorTests, SectorsCanLoadFromBlueprint)
{
	std::stringstream input;

	input << "weight = 12\n";
	input << "building_barracks building_fort\n";
	input << "multiply = {value = 0.1 vic3_law = law_quality}\n";
	input << "multiply = { value = 1.5 vic3_law = law_peasant_levy }\n";
	input << "add = { value = 1 is_colony = no }\n";

	V3::SectorLoader sectorBlueprint;
	sectorBlueprint.loadSector(input);

	const auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(std::make_shared<EU4::Country>());
	V3::ProcessedData data;
	data.laws.emplace("law_quality");
	data.laws.emplace("law_peasant_levy");
	data.CPBudget = 1000;
	country->setProcessedData(data);

	V3::Sector sector(sectorBlueprint, *country);

	EXPECT_THAT(sector.getBuildings(), testing::UnorderedElementsAre("building_barracks", "building_fort"));
	EXPECT_EQ(0, sector.getCPBudget());

	sector.calculateBudget(10, country->getCPBudget()); // The sector for this country should have a weight of 7.8

	EXPECT_EQ(780, sector.getCPBudget());
}