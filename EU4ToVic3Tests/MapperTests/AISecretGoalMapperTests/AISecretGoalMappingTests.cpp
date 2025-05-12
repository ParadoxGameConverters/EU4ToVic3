#include "AISecretGoalMapper/AISecretGoalMapping.h"
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

TEST(Mappers_AISecretGoalMappingTests, blankMappingReturnsTrue)
{
	const mappers::AISecretGoalMapping mapping;
	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnCapital)
{
	std::stringstream input;
	input << "capital = STATE_TEST_LAND1";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnCapital)
{
	std::stringstream input;
	input << "capital = STATE_TEST_LAND2";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnTargetCapital)
{
	std::stringstream input;
	input << "target_capital = STATE_TEST_LAND2";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnTargetCapital)
{
	std::stringstream input;
	input << "target_capital = STATE_TEST_LAND1";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnCapitalDifferentRegions)
{
	std::stringstream input;
	input << "target_capital_diff_region = no";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnCapitalDifferentRegions)
{
	std::stringstream input;
	input << "target_capital_diff_region = yes";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnTargetRankLEQ)
{
	std::stringstream input;
	input << "target_rank_leq = 5";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnTargetRankLEQ)
{
	std::stringstream input;
	input << "target_rank_leq = 0";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnTargetRankGEQ)
{
	std::stringstream input;
	input << "target_rank_geq = 0";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnTargetRankGEQ)
{
	std::stringstream input;
	input << "target_rank_geq = 6";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnSubject)
{
	std::stringstream input;
	input << "target_subject = yes";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnSubject)
{
	std::stringstream input;
	input << "target_subject = no";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnOverlord)
{
	std::stringstream input;
	input << "target_overlord = yes";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*target, *country, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnOverlord)
{
	std::stringstream input;
	input << "target_overlord = no";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*target, *country, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnRival)
{
	std::stringstream input;
	input << "target_is_rival = yes";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnRival)
{
	std::stringstream input;
	input << "target_is_rival = no";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnTargetGP)
{
	std::stringstream input;
	input << "target_gp = no";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnTargetGP)
{
	std::stringstream input;
	input << "target_gp = yes";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnGP)
{
	std::stringstream input;
	input << "gp = yes";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnGP)
{
	std::stringstream input;
	input << "gp = no";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnClaim)
{
	std::stringstream input;
	input << "target_is_claimed = yes";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnClaim)
{
	std::stringstream input;
	input << "target_is_claimed = no";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnRivalClaim)
{
	std::stringstream input;
	input << "target_is_claimed_by_rival = yes";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnRivalClaim)
{
	std::stringstream input;
	input << "target_is_claimed_by_rival = no";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, matchOnGovernmentFormDifferent)
{
	std::stringstream input;
	input << "gov_form_diff = yes";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_TRUE(mapping.matchGoal(*country, *target, clayManager));
}

TEST(Mappers_AISecretGoalMappingTests, failOnGovernmentFormDifferent)
{
	std::stringstream input;
	input << "gov_form_diff = no";
	const mappers::AISecretGoalMapping mapping(input);


	const auto& [country, target] = prepCountries();
	const auto& clayManager = prepClayManager();

	EXPECT_FALSE(mapping.matchGoal(*country, *target, clayManager));
}
