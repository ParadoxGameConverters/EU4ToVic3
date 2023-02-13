#include "AISecretGoalMapper/AISecretGoalMapper.h"
#include "ClayManager/ClayManager.h"
#include "ClayManager/State/SubState.h"
#include "CountryManager/EU4Country.h"
#include "PoliticalManager/Country/Country.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
std::pair<std::shared_ptr<V3::Country>, std::shared_ptr<V3::Country>> prepCountries()
{
	auto newCountry = std::make_shared<V3::Country>();
	V3::ProcessedData data;
	data.laws.emplace("law_presidential_republic");
	data.capitalStateName = "STATE_TEST_LAND1";
	data.tier = "city_state";
	data.rivals.emplace("GAT");
	data.rivals.emplace("TAT");
	newCountry->setProcessedData(data);
	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setGP();
	newCountry->setSourceCountry(srcCountry);
	newCountry->setTag("TAG");

	auto newCountry2 = std::make_shared<V3::Country>();
	V3::ProcessedData data2;
	data2.laws.emplace("law_theocracy");
	data2.capitalStateName = "STATE_TEST_LAND2";
	data2.tier = "hegemony";
	data2.overlordTag = "TAG";
	newCountry2->setProcessedData(data2);
	const auto srcCountry2 = std::make_shared<EU4::Country>();
	newCountry2->setSourceCountry(srcCountry2);
	newCountry2->setTag("GAT");

	const auto subState = std::make_shared<V3::SubState>();
	subState->addClaim("TAG");
	subState->addClaim("TAT");
	newCountry2->addSubState(subState);

	return std::pair{newCountry, newCountry2};
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


TEST(Mappers_AISecretGoalMapperTests, EmptyMapperReturnsNothing)
{
	const mappers::AISecretGoalMapper mapper;
	const auto& [country, target] = prepCountries();
	const auto clayManager = prepClayManager();

	EXPECT_FALSE(mapper.matchSecretGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMapperTests, SecretGoalCanBeGenerated)
{
	mappers::AISecretGoalMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/ai_secret_goal_map.txt");
	const auto& [country, target] = prepCountries();
	const auto clayManager = prepClayManager();

	const auto goal = mapper.matchSecretGoal(*country, *target, clayManager);

	ASSERT_TRUE(goal);
	EXPECT_EQ("conquer", *goal);
}
