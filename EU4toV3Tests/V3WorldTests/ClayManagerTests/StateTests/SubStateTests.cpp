#include "ClayManager/ClayManager.h"
#include "ClayManager/State/Province.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/StateModifier.h"
#include "ClayManager/State/SubState.h"
#include "CultureLoader/CultureLoader.h"
#include "Mappers/CultureMapper/CultureMapper.h"
#include "Mappers/ReligionMapper/ReligionMapper.h"
#include "PoliticalManager/Country/Country.h"
#include "ReligionLoader/ReligionLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_SubStateTests, OwnerTagCanBeSetAndRetrieved)
{
	V3::SubState subState;

	EXPECT_FALSE(subState.getOwnerTag());

	const auto owner = std::make_shared<V3::Country>();
	owner->setTag("AAA");
	subState.setOwner(owner);

	EXPECT_EQ("AAA", subState.getOwnerTag());
}

TEST(V3World_SubStateTests, HomeStateNameCanBeSetAndRetrieved)
{
	V3::SubState subState;

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	auto name = subState.getHomeStateName();
	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Attempted to access the name of a nullptr state from a substate. Returning empty name.)"));
	EXPECT_TRUE(name.empty());

	const auto state = std::make_shared<V3::State>();
	state->setStateName("TEST_NAME");
	subState.setHomeState(state);

	name = subState.getHomeStateName();

	EXPECT_EQ("TEST_NAME", name);
}

TEST(V3World_SubStateTests, SubStateCanConvertDemograhics)
{
	V3::SubState subState;
	auto state = std::make_shared<V3::State>();
	state->setStateName("STATE_TEST_LAND1");
	subState.setHomeState(state);
	const auto owner = std::make_shared<V3::Country>();
	owner->setTag("AAA");
	subState.setOwner(owner);

	// let's make some source data.
	auto ratio = EU4::PopRatio("culture4", "anglican");
	V3::SourceProvinceData data;
	data.popRatios.push_back(ratio);
	subState.setSourceProvinceData({std::pair(data, 1)});

	// prep mappers.
	V3::ClayManager clayManager;

	mappers::ReligionMapper religionMapper;
	religionMapper.loadMappingRules("TestFiles/configurables/religion_map.txt");
	// link = { vic3 = protestant eu4 = protestant eu4 = reformed eu4 = anglican } <-

	mappers::CultureMapper cultureMapper;
	cultureMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");
	// link = { vic3 = vculture4 eu4 = culture4 religion = religion_2 owner = TAG }
	// link = {vic3 = vculture5 eu4 = culture4 religion = religion_2}
	// link = {vic3 = vculture6 eu4 = culture4} <-

	EU4::CultureLoader cultureLoader;
	EU4::ReligionLoader religionLoader;

	subState.convertDemographics(clayManager, cultureMapper, religionMapper, cultureLoader, religionLoader);

	ASSERT_EQ(1, subState.getDemographics().size());
	auto demo = subState.getDemographics()[0];
	EXPECT_EQ("vculture6", demo.culture);
	EXPECT_EQ("protestant", demo.religion);
	EXPECT_EQ(1, demo.upperRatio);
	EXPECT_EQ(1, demo.middleRatio);
	EXPECT_EQ(1, demo.lowerRatio);
}

TEST(V3World_SubStateTests, SubStateCanGeneratePopsFromDemographics)
{
	V3::SubState subState;
	V3::Demographic demo1;
	demo1.culture = "cul1";
	demo1.religion = "rel1";
	demo1.upperRatio = 0.15; // total ratio sum 0.3
	demo1.middleRatio = 0.075;
	demo1.lowerRatio = 0.075;

	V3::Demographic demo2;
	demo2.culture = "cul2";
	demo2.religion = "rel2";
	demo2.upperRatio = 0.1; // total ratio sum 0.7
	demo2.middleRatio = 0.1;
	demo2.lowerRatio = 0.5;

	subState.setDemographics({demo1, demo2});

	subState.generatePops(1000);

	const auto& pops = subState.getSubStatePops().getPops();
	ASSERT_EQ(2, pops.size());
	const auto& pop1 = pops[0];
	const auto& pop2 = pops[1];

	EXPECT_EQ("cul1", pop1.getCulture());
	EXPECT_EQ("rel1", pop1.getReligion());
	EXPECT_EQ(300, pop1.getSize());

	EXPECT_EQ("cul2", pop2.getCulture());
	EXPECT_EQ("rel2", pop2.getReligion());
	EXPECT_EQ(700, pop2.getSize());
}

TEST(V3World_SubStateTests, TerrainFrequencyNormalizes)
{
	auto substate = V3::SubState();

	auto p0 = std::make_shared<V3::Province>();
	p0->setName("x112233");
	p0->setTerrain("desert");
	auto p1 = std::make_shared<V3::Province>();
	p1->setName("x445566");
	p1->setTerrain("plains");
	auto p2 = std::make_shared<V3::Province>();
	p2->setName("x778899");
	p2->setTerrain("plains");

	const V3::ProvinceMap provinces{{p0->getName(), p0}, {p1->getName(), p1}, {p2->getName(), p2}};
	substate.setProvinces(provinces);

	EXPECT_DOUBLE_EQ(1.0 / 3, substate.getTerrainFrequency("desert"));
	EXPECT_DOUBLE_EQ(2.0 / 3, substate.getTerrainFrequency("plains"));
}

TEST(V3World_SubStateTests, coastalFlagCountsAsTerrain)
{
	auto substate = V3::SubState();

	auto p0 = std::make_shared<V3::Province>();
	p0->setName("x112233");
	p0->setTerrain("desert");
	p0->setCoastal();
	auto p1 = std::make_shared<V3::Province>();
	p1->setName("x445566");
	p1->setTerrain("plains");
	auto p2 = std::make_shared<V3::Province>();
	p2->setName("x778899");
	p2->setTerrain("plains");

	const V3::ProvinceMap provinces{{p0->getName(), p0}, {p1->getName(), p1}, {p2->getName(), p2}};
	substate.setProvinces(provinces);

	EXPECT_DOUBLE_EQ(1.0 / 4, substate.getTerrainFrequency("desert"));
	EXPECT_DOUBLE_EQ(2.0 / 4, substate.getTerrainFrequency("plains"));
	EXPECT_DOUBLE_EQ(1.0 / 4, substate.getTerrainFrequency("coastal"));
}

TEST(V3World_SubStateTests, InfrastructureCalculationIsolateStateModifieres)
{
	auto country = std::make_shared<V3::Country>();
	auto state = std::make_shared<V3::State>();
	auto substate = V3::SubState();

	substate.setOwner(country);
	substate.setHomeState(state);

	std::stringstream input;
	input << "id = 3002\n";
	input << "provinces = { \"x112233\" }\n";
	input << "traits = { \"trait0\" \"trait1\"\n }";

	state->loadState(input);

	std::stringstream modifierInput0;
	modifierInput0 << "\tmodifier = {\n";
	modifierInput0 << "\t\tstate_infrastructure_add = 10\n";
	modifierInput0 << "\t}\n";

	auto modifier0 = std::make_shared<V3::StateModifier>();
	modifier0->loadStateModifier(modifierInput0);
	modifier0->setName("trait0");

	std::stringstream modifierInput1;
	modifierInput1 << "\tmodifier = {\n";
	modifierInput1 << "\t\tstate_infrastructure_mult = -0.2\n";
	modifierInput1 << "\t}\n";

	auto modifier1 = std::make_shared<V3::StateModifier>();
	modifier1->loadStateModifier(modifierInput1);
	modifier1->setName("trait1");

	substate.calculateInfrastructure(V3::StateModifiers{{modifier0->getName(), modifier0}, {modifier1->getName(), modifier1}});

	EXPECT_DOUBLE_EQ(10.4, substate.getInfrastructure());
}

// TODO(Gawquon): Finish test after tech is implemented
TEST(DISABLED_V3World_SubStateTests, InfrastructureCalculationIsolatePopFactor)
{
	auto country = std::make_shared<V3::Country>();
	auto state = std::make_shared<V3::State>();
	auto substate = V3::SubState();

	substate.setOwner(country);
	substate.setHomeState(state);

	V3::Demographic demo;
	demo.culture = "cul";
	demo.religion = "rel";
	demo.upperRatio = 0.2; // total ratio sum 1
	demo.middleRatio = 0.2;
	demo.lowerRatio = 0.6;

	substate.setDemographics({demo});
	substate.generatePops(10000);


	// country->setTech({"tech"});

	std::stringstream techInput;
	techInput << "\tmodifier = {\n";
	techInput << "\t\tstate_infrastructure_from_population_add = 0.2\n";
	techInput << "\t\tstate_infrastructure_from_population_max_add = 4\n";
	techInput << "\t}\n";

	// auto tech = std::make_shared<V3::Technology>();
	// tech->loadTechnology(techInput);
	// std::map<std::string, std::shared_ptr<V3::Technology>> techMap({"tech", tech});

	substate.calculateInfrastructure(V3::StateModifiers{});

	EXPECT_DOUBLE_EQ(3.2, substate.getInfrastructure());
}


TEST(V3World_SubStateTests, InfrastructureCalculationFactorsFromPrivateVariables)
{
	auto country = std::make_shared<V3::Country>();
	auto state = std::make_shared<V3::State>();
	auto substate = V3::SubState();

	substate.setOwner(country);
	substate.setHomeState(state);

	substate.calculateInfrastructure(V3::StateModifiers{});
	EXPECT_DOUBLE_EQ(3, substate.getInfrastructure());

	substate.setMarketCapital();
	substate.calculateInfrastructure(V3::StateModifiers{});
	EXPECT_DOUBLE_EQ(3.75, substate.getInfrastructure());

	substate.setUnincorporated();
	substate.calculateInfrastructure(V3::StateModifiers{});
	EXPECT_DOUBLE_EQ(3, substate.getInfrastructure());
}


TEST(V3World_SubStateTests, InfrastructureCalculationExcessNegativeModifiersCap)
{
	auto country = std::make_shared<V3::Country>();
	auto state = std::make_shared<V3::State>();
	auto substate = V3::SubState();

	substate.setOwner(country);
	substate.setHomeState(state);

	std::stringstream input;
	input << "id = 3002\n";
	input << "provinces = { \"x112233\" }\n";
	input << "traits = { \"trait0\" \"trait1\"\n }";

	state->loadState(input);

	std::stringstream modifierInput0;
	modifierInput0 << "\tmodifier = {\n";
	modifierInput0 << "\t\tstate_infrastructure_add = -10\n";
	modifierInput0 << "\t}\n";

	auto modifier0 = std::make_shared<V3::StateModifier>();
	modifier0->loadStateModifier(modifierInput0);
	modifier0->setName("trait0");

	std::stringstream modifierInput1;
	modifierInput1 << "\tmodifier = {\n";
	modifierInput1 << "\t\tstate_infrastructure_mult = -1.2\n";
	modifierInput1 << "\t}\n";

	auto modifier1 = std::make_shared<V3::StateModifier>();
	modifier1->loadStateModifier(modifierInput1);
	modifier1->setName("trait1");

	substate.calculateInfrastructure(V3::StateModifiers{{modifier0->getName(), modifier0}, {modifier1->getName(), modifier1}});

	EXPECT_DOUBLE_EQ(0.0, substate.getInfrastructure());
}