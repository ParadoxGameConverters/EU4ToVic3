#include "AIStrategyMapper/AIStrategyMapping.h"
#include "ClayManager/ClayManager.h"
#include "CountryManager/EU4Country.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
std::shared_ptr<V3::Country> prepCountry()
{
	auto newCountry = std::make_shared<V3::Country>();
	V3::ProcessedData data;
	data.laws.emplace("law_slavery_banned");
	data.laws.emplace("law_presidential_republic");
	data.capitalStateName = "STATE_TEST_LAND1";
	newCountry->setProcessedData(data);

	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setCelestialEmperor(true);
	srcCountry->setEmperor(true);

	newCountry->setSourceCountry(srcCountry);

	return newCountry;
}

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});

V3::ClayManager prepClayManager()
{

	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.loadTerrainsIntoProvinces(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();

	return clayManager;
}
} // namespace

TEST(Mappers_AIStrategyMappingTests, DefaultsDefaultToDefault)
{
	const mappers::AIStrategyMapping mapping;

	EXPECT_FALSE(mapping.isOverride());
}

TEST(Mappers_AIStrategyMappingTests, EmptyMappingReturnsZero)
{
	const mappers::AIStrategyMapping mapping;

	const auto clayManager = V3::ClayManager();
	const auto country = prepCountry();

	EXPECT_EQ(0, mapping.matchStrategy(*country, clayManager));
}

TEST(Mappers_AIStrategyMappingTests, LawsAddValueToStrategy)
{
	std::stringstream input;
	input << "laws = { law_slavery_banned = 25 law_presidential_republic = 10 }"; // matches both.
	const mappers::AIStrategyMapping mapping(input);
	const auto clayManager = V3::ClayManager();
	const auto country = prepCountry();

	EXPECT_EQ(35, mapping.matchStrategy(*country, clayManager));
}


TEST(Mappers_AIStrategyMappingTests, CapitalAddsValueToStrategy)
{
	std::stringstream input;
	input << "capital = { STATE_TEST_LAND1 = 25 STATE_TEST_LAND2 = 10 }"; // matches STATE_TEST_LAND1
	const mappers::AIStrategyMapping mapping(input);
	const auto clayManager = prepClayManager();
	const auto country = prepCountry();

	EXPECT_EQ(25, mapping.matchStrategy(*country, clayManager));
}

TEST(Mappers_AIStrategyMappingTests, StatusesAddValueToStrategy)
{
	std::stringstream input;
	input << "hremperor = 100 cemperor = 50 gp = 25 not_gp = 10"; // does NOT match gp, matches rest.
	const mappers::AIStrategyMapping mapping(input);
	const auto clayManager = prepClayManager();
	const auto country = prepCountry();

	EXPECT_EQ(160, mapping.matchStrategy(*country, clayManager));
}

TEST(Mappers_AIStrategyMappingTests, OverrideCanBeSet)
{
	std::stringstream input;
	input << "override = yes";
	const mappers::AIStrategyMapping mapping(input);

	EXPECT_TRUE(mapping.isOverride());
}
