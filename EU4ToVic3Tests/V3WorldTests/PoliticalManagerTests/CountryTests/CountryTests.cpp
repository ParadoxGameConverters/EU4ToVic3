#include "ClayManager/ClayManager.h"
#include "ClayManager/State/SubState.h"
#include "CommonFunctions.h"
#include "CountryManager/EU4Country.h"
#include "CountryTierMapper/CountryTierMapper.h"
#include "CultureMapper/CultureMapper.h"
#include "Loaders/CultureLoader/CultureLoader.h"
#include "Loaders/LawLoader/LawLoader.h"
#include "Loaders/LocLoader/LocalizationLoader.h"
#include "Loaders/LocalizationLoader/EU4LocalizationLoader.h"
#include "Loaders/ReligionLoader/ReligionLoader.h"
#include "Loaders/TechLoader/TechLoader.h"
#include "PoliticalManager/Country/Country.h"
#include "ReligionMapper/ReligionMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_CountryTests, DefaultsDefaultToDefaults)
{
	const V3::Country country;
	EXPECT_TRUE(country.getTag().empty());
	EXPECT_FALSE(country.getVanillaData());
	EXPECT_TRUE(country.getSubStates().empty());
	EXPECT_FALSE(country.getSourceCountry());
}

TEST(V3World_CountryTests, VanillaDataSpawnsEmptyWhenInitializedFromStream)
{
	std::stringstream input;
	V3::Country country;
	country.initializeCountry(input);

	ASSERT_TRUE(country.getVanillaData());
	EXPECT_TRUE(country.getVanillaData()->type.empty());
	EXPECT_TRUE(country.getVanillaData()->tier.empty());
	EXPECT_TRUE(country.getVanillaData()->cultures.empty());
	EXPECT_TRUE(country.getVanillaData()->religion.empty());
	EXPECT_TRUE(country.getVanillaData()->capitalStateName.empty());
	EXPECT_EQ(std::nullopt, country.getVanillaData()->color);
	EXPECT_FALSE(country.getVanillaData()->is_named_from_capital);
}

TEST(V3World_CountryTests, CountryCanBeInitializedViaVanillaData)
{
	std::stringstream input;
	input << "country_type = recognized\n ";
	input << "tier = empire\n ";
	input << "cultures = { cul1 cul2 }\n ";
	input << "religion = protestant\n ";
	input << "capital = STATE_TEST_1\n ";
	input << "color = {147 130 110}\n ";
	input << "is_named_from_capital = yes\n ";
	V3::Country country;
	country.initializeCountry(input);

	EXPECT_EQ("recognized", country.getVanillaData()->type);
	EXPECT_EQ("empire", country.getVanillaData()->tier);
	EXPECT_THAT(country.getVanillaData()->cultures, testing::UnorderedElementsAre("cul1", "cul2"));
	EXPECT_EQ("protestant", country.getVanillaData()->religion);
	EXPECT_EQ("STATE_TEST_1", country.getVanillaData()->capitalStateName);
	EXPECT_EQ(commonItems::Color(std::array{147, 130, 110}), country.getVanillaData()->color);
	EXPECT_TRUE(country.getVanillaData()->is_named_from_capital);
}

TEST(V3World_CountryTests, CountryTagCanBeSetAndGet)
{
	V3::Country country;
	country.setTag("TAG");
	EXPECT_EQ("TAG", country.getTag());
}

TEST(V3World_CountryTests, CountryCanImportColorFromEU4)
{
	const V3::ClayManager clayManager;

	V3::Country country;
	country.setTag("GAT");
	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setTag("TAG");
	srcCountry->setMapColor(commonItems::Color(std::array{1, 2, 3}));
	country.setSourceCountry(srcCountry);

	mappers::CultureMapper culMapper;
	country.convertFromEU4Country(clayManager, culMapper, {}, {}, {}, {}, {}, false);

	EXPECT_TRUE(country.getProcessedData().color);
	EXPECT_EQ(commonItems::Color(std::array{1, 2, 3}), country.getProcessedData().color);
}

TEST(V3World_CountryTests, CountryWillOverrideEU4ColorWithVic3Color)
{
	const V3::ClayManager clayManager;

	std::stringstream input;
	input << "color = { 4 5 6 }";
	V3::Country country;
	country.initializeCountry(input);
	country.setTag("GAT");

	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setTag("TAG");
	srcCountry->setMapColor(commonItems::Color(std::array{1, 2, 3}));
	country.setSourceCountry(srcCountry);

	mappers::CultureMapper culMapper;
	country.convertFromEU4Country(clayManager, culMapper, {}, {}, {}, {}, {}, false);

	EXPECT_TRUE(country.getProcessedData().color);
	EXPECT_EQ(commonItems::Color(std::array{4, 5, 6}), country.getProcessedData().color);
}

TEST(V3World_CountryTests, CountryWillNotOverrideVic3colorWithEU4ColorIfNone)
{
	const V3::ClayManager clayManager;

	std::stringstream input;
	input << "color = { 4 5 6 }";
	V3::Country country;
	country.initializeCountry(input);
	country.setTag("GAT");

	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setTag("TAG");
	country.setSourceCountry(srcCountry);

	mappers::CultureMapper culMapper;
	country.convertFromEU4Country(clayManager, culMapper, {}, {}, {}, {}, {}, false);

	EXPECT_TRUE(country.getProcessedData().color);
	EXPECT_EQ(commonItems::Color(std::array{4, 5, 6}), country.getProcessedData().color);
}

TEST(V3World_CountryTests, LocalizationsAreAvailable)
{
	const V3::ClayManager clayManager;

	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setTag("TAG");
	srcCountry->setLocalizationName("english", "Name1");
	srcCountry->setLocalizationName("albanian", "Name2");
	srcCountry->setLocalizationName("alien", "Name3");
	srcCountry->setLocalizationAdjective("english", "Adj1");
	srcCountry->setLocalizationAdjective("albanian", "Adj2");
	srcCountry->setLocalizationAdjective("alien", "Adj3");

	V3::Country country;
	country.setTag("GAT");
	country.setSourceCountry(srcCountry);
	mappers::CultureMapper culMapper;
	country.convertFromEU4Country(clayManager, culMapper, {}, {}, {}, {}, {}, false);

	EXPECT_EQ("Name1", country.getName("english"));
	EXPECT_EQ("Name2", country.getName("albanian"));
	EXPECT_EQ("Name3", country.getName("alien"));
	EXPECT_EQ("Adj1", country.getAdjective("english"));
	EXPECT_EQ("Adj2", country.getAdjective("albanian"));
	EXPECT_EQ("Adj3", country.getAdjective("alien"));
}

TEST(V3World_CountryTests, MissingLocalizationsDefaultToEnglish)
{
	const V3::ClayManager clayManager;

	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setTag("TAG");
	srcCountry->setLocalizationName("english", "Name1");
	srcCountry->setLocalizationAdjective("english", "Adj1");

	V3::Country country;
	country.setTag("GAT");
	country.setSourceCountry(srcCountry);
	mappers::CultureMapper culMapper;
	country.convertFromEU4Country(clayManager, culMapper, {}, {}, {}, {}, {}, false);

	EXPECT_EQ("Name1", country.getName("english"));
	EXPECT_EQ("Name1", country.getName("albanian"));
	EXPECT_EQ("Name1", country.getName("alien"));
	EXPECT_EQ("Adj1", country.getAdjective("english"));
	EXPECT_EQ("Adj1", country.getAdjective("albanian"));
	EXPECT_EQ("Adj1", country.getAdjective("alien"));
}

TEST(V3World_CountryTests, AbsentLocalizationsDefaultToTag)
{
	const V3::ClayManager clayManager;

	const auto srcCountry = std::make_shared<EU4::Country>();
	srcCountry->setTag("TAG");

	V3::Country country;
	country.setTag("GAT");
	country.setSourceCountry(srcCountry);
	mappers::CultureMapper culMapper;
	country.convertFromEU4Country(clayManager, culMapper, {}, {}, {}, {}, {}, false);

	EXPECT_EQ("GAT", country.getName("english"));
	EXPECT_EQ("GAT_ADJ", country.getAdjective("english"));
}

TEST(V3World_CountryTests, CountryCanSpeedCopyVanillaData)
{
	std::stringstream input;
	input << "country_type = recognized\n ";
	input << "tier = empire\n ";
	input << "cultures = { cul1 cul2 }\n ";
	input << "religion = protestant\n ";
	input << "capital = STATE_TEST_1\n ";
	input << "color = {147 130 110}\n ";
	input << "is_named_from_capital = yes\n ";
	V3::Country country;
	country.setTag("AAA");
	country.initializeCountry(input);

	V3::LocalizationLoader locLoader;
	std::stringstream locInput;
	locInput << commonItems::utf8BOM << "l_english:\n";
	locInput << " AAA: \"The country\"\n";
	locInput << " AAA_ADJ: \"The adjective\"\n";
	locLoader.scrapeStream(locInput, "english");

	country.copyVanillaData(locLoader, EU4::EU4LocalizationLoader());

	EXPECT_EQ("recognized", country.getProcessedData().type);
	EXPECT_EQ("empire", country.getProcessedData().tier);
	EXPECT_THAT(country.getProcessedData().cultures, testing::UnorderedElementsAre("cul1", "cul2"));
	EXPECT_EQ("protestant", country.getProcessedData().religion);
	EXPECT_EQ("STATE_TEST_1", country.getProcessedData().capitalStateName);
	EXPECT_EQ(commonItems::Color(std::array{147, 130, 110}), country.getProcessedData().color);
	EXPECT_TRUE(country.getProcessedData().is_named_from_capital);
	EXPECT_EQ("The country", country.getProcessedData().namesByLanguage.at("english"));
	EXPECT_EQ("The adjective", country.getProcessedData().adjectivesByLanguage.at("english"));
}

TEST(V3World_CountryTests, CountryWontSpeedCopyMissingVanillaData)
{
	V3::Country country;

	country.copyVanillaData(V3::LocalizationLoader(), EU4::EU4LocalizationLoader());

	EXPECT_TRUE(country.getProcessedData().type.empty());
	EXPECT_TRUE(country.getProcessedData().tier.empty());
	EXPECT_TRUE(country.getProcessedData().cultures.empty());
	EXPECT_TRUE(country.getProcessedData().religion.empty());
	EXPECT_TRUE(country.getProcessedData().capitalStateName.empty());
	EXPECT_FALSE(country.getProcessedData().color);
	EXPECT_FALSE(country.getProcessedData().is_named_from_capital);
}

TEST(V3World_CountryTests, PopCounterSumsSubStatePop)
{
	V3::Country country;
	std::vector<std::shared_ptr<V3::SubState>> substates;

	EXPECT_EQ(0, country.getPopCount());
	EXPECT_EQ(0, V3::Country::getPopCount(substates));

	auto sub0 = std::make_shared<V3::SubState>();
	auto sub1 = std::make_shared<V3::SubState>();

	V3::Pop pop0, pop1;
	pop0.setSize(1000);
	pop1.setSize(500);

	sub0->addPop(pop0);
	sub1->addPop(pop1);

	substates.push_back(sub0);
	substates.push_back(sub1);

	EXPECT_EQ(0, country.getPopCount());
	EXPECT_EQ(1500, V3::Country::getPopCount(substates));

	country.addSubState(sub0);
	country.addSubState(sub1);

	EXPECT_EQ(1500, country.getPopCount());
}

TEST(V3World_CountryTests, CultureBreakdownSumsSubStateCultures)
{
	V3::Country country;
	std::vector<std::shared_ptr<V3::SubState>> substates;

	EXPECT_EQ(0, country.getPopCount());
	EXPECT_EQ(0, V3::Country::getPopCount(substates));

	auto sub0 = std::make_shared<V3::SubState>();
	auto sub1 = std::make_shared<V3::SubState>();

	V3::Pop pop0("english", "", "", 1000);
	V3::Pop pop1("german", "", "", 500);
	V3::Pop pop2("english", "", "", 500);

	sub0->addPop(pop0);
	sub1->addPop(pop1);
	sub1->addPop(pop2);

	substates.push_back(sub0);
	substates.push_back(sub1);

	country.addSubState(sub0);
	country.addSubState(sub1);

	EXPECT_THAT(country.getCultureBreakdown(), testing::UnorderedElementsAre(testing::Pair("english", 0.75), testing::Pair("german", 0.25)));
}

TEST(V3World_CountryTests, InfrastructureFromCountryTech)
{
	V3::Country country;

	V3::TechLoader techLoader;
	techLoader.loadTechs(modFS);

	V3::ProcessedData data;
	data.techs.emplace("tech_1"); // Mult 20%, Max of 40
	data.techs.emplace("tech_2"); // Mult 5%, Max of 10
	data.techs.emplace("tech_4"); // No effect

	country.setProcessedData(data);

	EXPECT_EQ(50, country.getTechInfraCap(techLoader.getTechs()));
	EXPECT_DOUBLE_EQ(0.25, country.getTechInfraMult(techLoader.getTechs()));
}

TEST(V3World_CountryTests, GetGovernmentBuildingMaxCapacities)
{
	V3::Country country;

	V3::LawLoader lawLoader;
	lawLoader.loadLaws(modFS);

	V3::TechLoader techLoader;
	techLoader.loadTechs(modFS);


	V3::ProcessedData data;
	data.laws.emplace("law_2");	// None
	data.laws.emplace("law_3");	// 70 Barracks
	data.laws.emplace("law_4");	// 30 Barracks
	data.techs.emplace("tech_2"); // 15 Bases, 2 Ports
	data.techs.emplace("tech_3"); // None
	data.techs.emplace("tech_4"); // 10 Bases, 4 Ports

	country.setProcessedData(data);

	EXPECT_EQ(100, country.getGovBuildingMax("building_barracks", lawLoader.getLaws(), techLoader.getTechs()));
	EXPECT_EQ(25, country.getGovBuildingMax("building_naval_base", lawLoader.getLaws(), techLoader.getTechs()));
	EXPECT_EQ(6, country.getGovBuildingMax("building_port", lawLoader.getLaws(), techLoader.getTechs()));
}

TEST(V3World_CountryTests, YearCapFactorHitsCurve)
{
	EXPECT_NEAR(1, V3::Country::yearCapFactor(date("1836.1.1")), 0.01);
	EXPECT_NEAR(0.85, V3::Country::yearCapFactor(date("1821.1.1")), 0.01);
	EXPECT_NEAR(0.5, V3::Country::yearCapFactor(date("1750.1.1")), 0.01);
	EXPECT_NEAR(0.3, V3::Country::yearCapFactor(date("1650.1.1")), 0.015);
	EXPECT_NEAR(0.2, V3::Country::yearCapFactor(date("1490.1.1")), 0.01);
	EXPECT_NEAR(0.15, V3::Country::yearCapFactor(date("1350.1.1")), 0.01);
}

TEST(V3World_CountryTests, HasAnyOfLawUnlockingIsTrueIfAnyOfLawsArePresent)
{
	V3::Country country;

	V3::ProcessedData data;
	data.laws.emplace("law_2");
	data.laws.emplace("law_3");
	data.laws.emplace("law_4");

	country.setProcessedData(data);

	EXPECT_TRUE(country.hasAnyOfLawUnlocking({"law_2"}));
	EXPECT_TRUE(country.hasAnyOfLawUnlocking({"law_1", "law_2"}));
	EXPECT_TRUE(country.hasAnyOfLawUnlocking({"law_2", "law_3"}));
}

TEST(V3World_CountryTests, HasAnyOfLawUnlockingIsTrueIfLawsAreEmpty)
{
	V3::Country country;

	V3::ProcessedData data;
	data.laws.emplace("law_2");
	data.laws.emplace("law_3");
	data.laws.emplace("law_4");

	country.setProcessedData(data);

	EXPECT_TRUE(country.hasAnyOfLawUnlocking({}));
}

TEST(V3World_CountryTests, HasAnyOfLawUnlockingFalseIfNoneOfLawsPresent)
{
	V3::Country country;

	V3::ProcessedData data;
	data.laws.emplace("law_2");
	data.laws.emplace("law_3");
	data.laws.emplace("law_4");

	country.setProcessedData(data);

	EXPECT_FALSE(country.hasAnyOfLawUnlocking({"law_1"}));
	EXPECT_FALSE(country.hasAnyOfLawUnlocking({"law_1", "law_5"}));
}

TEST(V3World_CountryTests, HasAnyOfLawBlockingTrueIfAnyOfLawsArePresent)
{
	V3::Country country;

	V3::ProcessedData data;
	data.laws.emplace("law_2");
	data.laws.emplace("law_3");
	data.laws.emplace("law_4");

	country.setProcessedData(data);

	EXPECT_TRUE(country.hasAnyOfLawBlocking({"law_2"}));
	EXPECT_TRUE(country.hasAnyOfLawBlocking({"law_1", "law_2"}));
	EXPECT_TRUE(country.hasAnyOfLawBlocking({"law_2", "law_3"}));
}

TEST(V3World_CountryTests, HasAnyOfLawBlockingFalseIfLawsAreEmpty)
{
	V3::Country country;

	V3::ProcessedData data;

	country.setProcessedData(data);

	EXPECT_FALSE(country.hasAnyOfLawBlocking({}));
}

TEST(V3World_CountryTests, HasAnyOfLawBlockingFalseIfNoneOfLawsPresent)
{
	V3::Country country;

	V3::ProcessedData data;
	data.laws.emplace("law_2");
	data.laws.emplace("law_3");
	data.laws.emplace("law_4");

	country.setProcessedData(data);

	EXPECT_FALSE(country.hasAnyOfLawBlocking({"law_1"}));
	EXPECT_FALSE(country.hasAnyOfLawBlocking({"law_1", "law_5"}));
}

TEST(V3World_CountryTests, GetCultureBreakdownReturnsPopulationFractions)
{
	V3::Country country;

	std::shared_ptr<V3::SubState> subState = std::make_shared<V3::SubState>();
	subState->addPop(V3::Pop("german", "no_religion", "", 650));
	subState->addPop(V3::Pop("khmer", "crusader_kings_two", "", 250));

	std::shared_ptr<V3::SubState> subState2 = std::make_shared<V3::SubState>();
	subState2->addPop(V3::Pop("english", "no_religion", "", 100));

	country.addSubState(subState);
	country.addSubState(subState2);

	EXPECT_THAT(country.getCultureBreakdown(),
		 testing::UnorderedElementsAre(testing::Pair("german", 0.65), testing::Pair("khmer", 0.25), testing::Pair("english", 0.1)));
}

TEST(V3World_CountryTests, GetJobBreakdownReturnsPopTypeFractions)
{
	V3::Country country;

	std::shared_ptr<V3::SubState> subState = std::make_shared<V3::SubState>();
	subState->addJob("peasants", 500);
	subState->addJob("machinists", 400);
	subState->addPop(V3::Pop("german", "no_religion", "", 650));
	subState->addPop(V3::Pop("khmer", "crusader_kings_two", "", 250));


	std::shared_ptr<V3::SubState> subState2 = std::make_shared<V3::SubState>();
	subState2->addJob("laborers", 100);
	subState2->addPop(V3::Pop("english", "no_religion", "", 100));

	country.addSubState(subState);
	country.addSubState(subState2);

	EXPECT_THAT(country.getJobBreakdown(),
		 testing::UnorderedElementsAre(testing::Pair("peasants", 0.5), testing::Pair("machinists", 0.4), testing::Pair("laborers", 0.1)));
}