#include "AIStrategyMapper/AIStrategyMapper.h"
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
	data.laws.emplace("law_national_supremacy");
	data.laws.emplace("law_serfdom_banned");
	data.laws.emplace("law_hereditary_bureaucrats");
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

TEST(Mappers_AIStrategyMapperTests, EmptyMapperReturnsNothing)
{
	const mappers::AIStrategyMapper mapper;
	const auto country = prepCountry();
	const auto clayManager = prepClayManager();

	EXPECT_TRUE(mapper.getAdmStrategies(*country, clayManager).empty());
	EXPECT_TRUE(mapper.getDipStrategies(*country, clayManager).empty());
	EXPECT_TRUE(mapper.getPolStrategies(*country, clayManager).empty());
}

TEST(Mappers_AIStrategyMapperTests, StrategiesCanBeMatchedAndOverridden)
{
	mappers::AIStrategyMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/ai_strategy_map.txt");
	const auto country = prepCountry();
	const auto clayManager = prepClayManager();

	const auto adm = mapper.getAdmStrategies(*country, clayManager);
	const auto dip = mapper.getDipStrategies(*country, clayManager);
	const auto pol = mapper.getPolStrategies(*country, clayManager);


	// law_serfdom_banned -> ai_strategy_industrial_expansion=50
	// law_slavery_banned -> ai_strategy_resource_expansion=25
	EXPECT_THAT(adm, testing::UnorderedElementsAre(std::pair("ai_strategy_industrial_expansion", 50), std::pair("ai_strategy_resource_expansion", 25)));

	// hremperor -> ai_strategy_maintain_power_balance=100
	// not_gp -> ai_strategy_maintain_power_balance=50
	// law_national_supremacy -> ai_strategy_territorial_expansion=0
	EXPECT_THAT(dip, testing::UnorderedElementsAre(std::pair("ai_strategy_maintain_power_balance", 150), std::pair("ai_strategy_territorial_expansion", 25)));

	// cemperor -> ai_strategy_maintain_mandate_of_heaven=50 OVERRIDE
	// law_hereditary_bureaucrats -> ai_strategy_conservative_agenda=25 overridden
	// law_presidential_republic -> ai_strategy_progressive_agenda=25 overridden
	EXPECT_THAT(pol, testing::UnorderedElementsAre(std::pair("ai_strategy_maintain_mandate_of_heaven", 0)));
}

TEST(Mappers_AIStrategyMapperTests, StrategiesCanDefault)
{
	mappers::AIStrategyMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/ai_strategy_map.txt");
	const auto country = std::make_shared<V3::Country>(); // no data inside.
	const auto clayManager = prepClayManager();

	const auto adm = mapper.getAdmStrategies(*country, clayManager);
	const auto dip = mapper.getDipStrategies(*country, clayManager);
	const auto pol = mapper.getPolStrategies(*country, clayManager);

	EXPECT_THAT(adm, testing::UnorderedElementsAre(std::pair("ai_strategy_resource_expansion", 0)));
	EXPECT_THAT(dip, testing::UnorderedElementsAre(std::pair("ai_strategy_maintain_power_balance", 0)));
	EXPECT_THAT(pol, testing::UnorderedElementsAre(std::pair("ai_strategy_conservative_agenda", 0)));
}