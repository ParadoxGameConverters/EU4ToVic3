#include "BuildingMapper/ProductionMethodMapper/ProductionMethodMapper.h"
#include "ClayManager/State/SubState.h"
#include "EconomyManager/Building/Building.h"
#include "Loaders/BuildingLoader/ProductionMethodLoader/ProductionMethodGroupLoader.h"
#include "Loaders/BuildingLoader/ProductionMethodLoader/ProductionMethodLoader.h"
#include "ModLoader/ModFilesystem.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
#include <numeric>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game", {});
std::pair<std::map<std::string, V3::ProductionMethod>, std::map<std::string, V3::ProductionMethodGroup>> prepPMData()
{
	V3::ProductionMethodLoader PMLoader;
	PMLoader.loadPMs(modFS);

	V3::ProductionMethodGroupLoader PMGroupLoader;
	PMGroupLoader.loadPMGroups(modFS);

	return {PMLoader.getPMs(), PMGroupLoader.getPMGroups()};
}

// Tests for specific PMs
auto sumSawmills = [](const int sum, const std::shared_ptr<V3::SubState> s) {
	if (s->getBuildings()[0]->getPMs().contains("pm_saw_mills"))
		return sum + s->getBuildings()[0]->getLevel(); // Only built logging camps
	return sum;
};

auto sumHardwoods = [](const int sum, const std::shared_ptr<V3::SubState> s) {
	if (s->getBuildings()[0]->getPMs().contains("pm_hardwood"))
		return sum + s->getBuildings()[0]->getLevel(); // Only built logging camps
	return sum;
};

auto sumForestry = [](const int sum, const std::shared_ptr<V3::SubState> s) {
	if (s->getBuildings()[0]->getPMs().contains("pm_simple_forestry"))
		return sum + s->getBuildings()[0]->getLevel(); // Only built logging camps
	return sum;
};

// Building pmgs
const std::set<std::string> lumberPmgs = {"pmg_base_building_logging_camp",
	 "pmg_hardwood",
	 "pmg_equipment",
	 "pmg_transportation_building_logging_camp",
	 "pmg_ownership_capital_building_logging_camp"};

const std::set<std::string> subsistencePmgs = {"pmg_base_building_subsistence_farm",
	 "pmg_home_workshops_building_subsistence_farm",
	 "pmg_serfdom_building_subsistence_farm"};
} // namespace

TEST(Mappers_ProductionMethodMapperTests, RulesCanBeLoaded)
{
	mappers::ProductionMethodMapper mapper;

	EXPECT_TRUE(mapper.getRules().empty());

	mapper.loadRules("TestFiles/configurables/economy/production_method_rules.txt");

	EXPECT_FALSE(mapper.getRules().contains("building_government_administration"));
	EXPECT_THAT(mapper.getRules().at("building_logging_camp"),
		 testing::UnorderedElementsAre(mappers::PMRule{"pm_saw_mills"},
			  mappers::PMRule{"pm_hardwood", 0.65},
			  mappers::PMRule{"pm_merchant_guilds_building_logging_camp", 1, true}));
}
TEST(Mappers_ProductionMethodMapperTests, ApplyRules)
{
	mappers::ProductionMethodMapper mapper;
	mapper.loadRules("TestFiles/configurables/economy/production_method_rules.txt");

	// Set up a country with many subStates with lumber camps
	V3::Country country;
	country.addTech("steelworking");				// Allows sawmills
	country.addTech("electrical_generation"); // Allows PMs after sawmills

	for (const int lvl: {3, 12, 14, 2, 7, 8, 9, 7, 5, 3, 5, 7, 1, 4, 6, 8, 4, 2, 1, 15}) // TN: 79
	{
		auto subState = std::make_shared<V3::SubState>();
		auto lumberCamp = std::make_shared<V3::Building>();
		lumberCamp->setName("building_logging_camp");
		lumberCamp->setPMGroups(lumberPmgs);
		lumberCamp->setLevel(lvl);
		subState->addBuilding(lumberCamp);
		country.addSubState(subState);
	}

	// Load in PM and PMGroup definitions
	const auto [PMs, PMGroups] = prepPMData();
	mapper.applyRules(country, PMs, PMGroups);

	EXPECT_EQ(123, std::accumulate(country.getSubStates().begin(), country.getSubStates().end(), 0, sumSawmills));
	EXPECT_EQ(79, std::accumulate(country.getSubStates().begin(), country.getSubStates().end(), 0, sumHardwoods));
}
TEST(Mappers_ProductionMethodMapperTests, ApplyRulesInexactOverClosest)
{
	mappers::ProductionMethodMapper mapper;
	mapper.loadRules("TestFiles/configurables/economy/production_method_rules.txt");

	// Set up a country with many subStates with lumber camps
	V3::Country country;

	for (const int lvl: {22, 22, 18, 22}) // TN: 54, U: 44, O: 62, Over is closer.
	{
		auto subState = std::make_shared<V3::SubState>();
		auto lumberCamp = std::make_shared<V3::Building>();
		lumberCamp->setName("building_logging_camp");
		lumberCamp->setPMGroups(lumberPmgs);
		lumberCamp->setLevel(lvl);
		subState->addBuilding(lumberCamp);
		country.addSubState(subState);
	}

	// Load in PM and PMGroup definitions
	const auto [PMs, PMGroups] = prepPMData();
	mapper.applyRules(country, PMs, PMGroups);

	EXPECT_EQ(62, std::accumulate(country.getSubStates().begin(), country.getSubStates().end(), 0, sumHardwoods));
}
TEST(Mappers_ProductionMethodMapperTests, ApplyRulesInexactUnderClosest)
{
	mappers::ProductionMethodMapper mapper;
	mapper.loadRules("TestFiles/configurables/economy/production_method_rules.txt");

	// Set up a country with many subStates with lumber camps
	V3::Country country;

	for (const int lvl: {15, 3, 15, 15}) // TN: 31, U: 30, O: 33. Under is closer.
	{
		auto subState = std::make_shared<V3::SubState>();
		auto lumberCamp = std::make_shared<V3::Building>();
		lumberCamp->setName("building_logging_camp");
		lumberCamp->setPMGroups(lumberPmgs);
		lumberCamp->setLevel(lvl);
		subState->addBuilding(lumberCamp);
		country.addSubState(subState);
	}

	// Load in PM and PMGroup definitions
	const auto [PMs, PMGroups] = prepPMData();
	mapper.applyRules(country, PMs, PMGroups);

	EXPECT_EQ(30, std::accumulate(country.getSubStates().begin(), country.getSubStates().end(), 0, sumHardwoods));
}
TEST(Mappers_ProductionMethodMapperTests, ApplyRulesSmallestLargeBuildingIsClosest)
{
	mappers::ProductionMethodMapper mapper;
	mapper.loadRules("TestFiles/configurables/economy/production_method_rules.txt");

	// Set up a country with many subStates with lumber camps
	V3::Country country;

	for (const int lvl: {9, 91}) // TN: 65, 91 is closer to 65 than 9, singe buildings larger than the TN are special cases.
	{
		auto subState = std::make_shared<V3::SubState>();
		auto lumberCamp = std::make_shared<V3::Building>();
		lumberCamp->setName("building_logging_camp");
		lumberCamp->setPMGroups(lumberPmgs);
		lumberCamp->setLevel(lvl);
		subState->addBuilding(lumberCamp);
		country.addSubState(subState);
	}

	// Load in PM and PMGroup definitions
	const auto [PMs, PMGroups] = prepPMData();
	mapper.applyRules(country, PMs, PMGroups);

	EXPECT_EQ(91, std::accumulate(country.getSubStates().begin(), country.getSubStates().end(), 0, sumHardwoods));
}
TEST(Mappers_ProductionMethodMapperTests, ApplyRulesUnderTeched)
{
	mappers::ProductionMethodMapper mapper;
	mapper.loadRules("TestFiles/configurables/economy/production_method_rules.txt");

	// Set up a country with many subStates with lumber camps
	V3::Country country;

	for (const int lvl: {3, 12, 14, 2, 7, 8, 9, 7, 5, 3, 5, 7, 1, 4, 6, 8, 4, 2, 1, 15})
	{
		auto subState = std::make_shared<V3::SubState>();
		auto lumberCamp = std::make_shared<V3::Building>();
		lumberCamp->setName("building_logging_camp");
		lumberCamp->setPMGroups(lumberPmgs);
		lumberCamp->setLevel(lvl);
		subState->addBuilding(lumberCamp);
		country.addSubState(subState);
	}

	// Load in PM and PMGroup definitions
	const auto [PMs, PMGroups] = prepPMData();
	mapper.applyRules(country, PMs, PMGroups);

	// No tech for sawmills
	EXPECT_EQ(0, std::accumulate(country.getSubStates().begin(), country.getSubStates().end(), 0, sumSawmills));
	EXPECT_EQ(123, std::accumulate(country.getSubStates().begin(), country.getSubStates().end(), 0, sumForestry));
}
TEST(Mappers_ProductionMethodMapperTests, EstimatesWalkPMList)
{
	mappers::ProductionMethodMapper mapper;
	mapper.loadRules("TestFiles/configurables/economy/production_method_rules.txt");

	// Set up countries with no, partial, and full tech
	V3::Country countryNoTech;
	V3::Country countrySomeTech;
	V3::Country countryAllTech;

	countrySomeTech.addTech("steelworking");
	countryAllTech.addTech("steelworking");
	countryAllTech.addTech("electrical_generation");

	// Prepare Buildings
	V3::Building lumberCamp;
	lumberCamp.setName("building_logging_camp");
	lumberCamp.setPMGroups(lumberPmgs);

	std::map<std::string, V3::Building> buildingMap;
	buildingMap.emplace("building_logging_camp", lumberCamp);

	// Load in PM and PMGroup definitions
	const auto [PMs, PMGroups] = prepPMData();

	const auto& lowPMEstimates = mapper.estimatePMs(countryNoTech, PMs, PMGroups, buildingMap);
	const auto& midPMEstimates = mapper.estimatePMs(countrySomeTech, PMs, PMGroups, buildingMap);
	const auto& highPMEstimates = mapper.estimatePMs(countryAllTech, PMs, PMGroups, buildingMap);

	// The rule says advance to saw_mills, not electric_sawmills

	EXPECT_THAT(lowPMEstimates, testing::Contains(testing::Pair("pmg_base_building_logging_camp", std::make_tuple(0, 1.0))));
	EXPECT_THAT(midPMEstimates, testing::Contains(testing::Pair("pmg_base_building_logging_camp", std::make_tuple(1, 1.0))));
	EXPECT_THAT(highPMEstimates, testing::Contains(testing::Pair("pmg_base_building_logging_camp", std::make_tuple(1, 1.0))));

	/*
	 * pmg_base_building_logging_camp = {
	 *	production_methods = {
	 *		pm_simple_forestry // countryNoTech
	 *		pm_saw_mills // countrySomeTech & countryAllTech
	 *		pm_electric_saw_mills // not countryAllTech because the rule in mapper.loadRules() limits the highest PM to saw_mills
	 *		}
	 * }
	 */
}
TEST(Mappers_ProductionMethodMapperTests, ConfigFlagSwitchesWalkType)
{
	mappers::ProductionMethodMapper mapper;
	mapper.loadRules("TestFiles/configurables/economy/production_method_rules.txt");

	// Set up a country using command economy to use the 4th pm in the pmg.
	V3::Country countryNoTech;
	countryNoTech.addLaw("law_command_economy");

	// Prepare Buildings
	V3::Building lumberCamp;
	lumberCamp.setName("building_logging_camp");
	lumberCamp.setPMGroups(lumberPmgs);

	std::map<std::string, V3::Building> buildingMap;
	buildingMap.emplace("building_logging_camp", lumberCamp);

	// Load in PM and PMGroup definitions
	const auto [PMs, PMGroups] = prepPMData();

	const auto& lawPMEstimates = mapper.estimatePMs(countryNoTech, PMs, PMGroups, buildingMap);

	EXPECT_THAT(lawPMEstimates, testing::Contains(testing::Pair("pmg_ownership_capital_building_logging_camp", std::make_tuple(3, 1.0))));

	/*
	 * pmg_ownership_capital_building_logging_camp = {
	 *	production_methods = {
	 *		pm_merchant_guilds_building_logging_camp
	 *		pm_privately_owned_building_logging_camp
	 *		pm_publicly_traded_building_logging_camp
	 *		pm_government_run_building_logging_camp // this one
	 *		pm_worker_cooperative_building_logging_camp
	 *		}
	 * }
	 */
}
TEST(Mappers_ProductionMethodMapperTests, EstimatesWalkPMListLawMethod)
{
	mappers::ProductionMethodMapper mapper;
	mapper.loadRules("TestFiles/configurables/economy/production_method_rules.txt");

	// Set up countries with different peasant laws
	V3::Country countrySerfdom;
	V3::Country countryHomesteading;

	countrySerfdom.addLaw("law_serfdom");
	countryHomesteading.addLaw("law_homesteading");

	// Prepare Buildings
	V3::Building subsistence;
	subsistence.setName("building_subsistence_farm");
	subsistence.setPMGroups(subsistencePmgs);

	std::map<std::string, V3::Building> buildingMap;
	buildingMap.emplace("building_subsistence_farm", subsistence);

	// Load in PM and PMGroup definitions
	const auto [PMs, PMGroups] = prepPMData();

	const auto& serfPMEstimates = mapper.estimatePMs(countrySerfdom, PMs, PMGroups, buildingMap);
	const auto& homesteadPMEstimates = mapper.estimatePMs(countryHomesteading, PMs, PMGroups, buildingMap);

	EXPECT_THAT(serfPMEstimates, testing::Contains(testing::Pair("pmg_serfdom_building_subsistence_farm", std::make_tuple(1, 1.0))));
	EXPECT_THAT(homesteadPMEstimates, testing::Contains(testing::Pair("pmg_serfdom_building_subsistence_farm", std::make_tuple(2, 1.0))));

	/*
	 * pmg_serfdom_building_subsistence_farm  = {
	 *	production_methods = {
	 *		pm_serfdom_no
	 *		pm_serfdom // countrySerfdom
	 *		pm_homesteading_building_subsistence // countryHomesteading
	 *		}
	 * }
	 */
}