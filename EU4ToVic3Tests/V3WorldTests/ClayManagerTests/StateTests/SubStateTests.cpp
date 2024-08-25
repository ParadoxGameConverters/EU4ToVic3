#include "ClayManager/ClayManager.h"
#include "ClayManager/State/Province.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/StateModifier.h"
#include "ClayManager/State/SubState.h"
#include "CultureLoader/CultureLoader.h"
#include "Loaders/TechLoader/TechLoader.h"
#include "Mappers/CultureMapper/CultureMapper.h"
#include "Mappers/ReligionMapper/ReligionMapper.h"
#include "PoliticalManager/Country/Country.h"
#include "ReligionLoader/ReligionLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

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

	subState.generatePops(1000, 200);

	const auto& pops = subState.getSubStatePops().getPops();
	ASSERT_EQ(4, pops.size());
	const auto& pop1 = pops[0];
	const auto& pop2 = pops[1];
	const auto& pop3 = pops[2];
	const auto& pop4 = pops[3];

	EXPECT_EQ("cul1", pop1.getCulture());
	EXPECT_EQ("rel1", pop1.getReligion());
	EXPECT_TRUE(pop1.getType().empty());
	EXPECT_EQ(240, pop1.getSize());

	EXPECT_EQ("cul1", pop2.getCulture());
	EXPECT_EQ("rel1", pop2.getReligion());
	EXPECT_EQ("slaves", pop2.getType());
	EXPECT_EQ(60, pop2.getSize());

	EXPECT_EQ("cul2", pop3.getCulture());
	EXPECT_EQ("rel2", pop3.getReligion());
	EXPECT_TRUE(pop3.getType().empty());
	EXPECT_EQ(560, pop3.getSize());

	EXPECT_EQ("cul2", pop4.getCulture());
	EXPECT_EQ("rel2", pop4.getReligion());
	EXPECT_EQ("slaves", pop4.getType());
	EXPECT_EQ(140, pop4.getSize());
}

TEST(V3World_SubStateTests, SubStateCanReturnPrimaryCulture)
{
	V3::SubState subState;
	V3::Demographic demo1;
	demo1.culture = "cul1";
	demo1.religion = "rel1";
	demo1.upperRatio = 0.15;
	demo1.middleRatio = 0.075;
	demo1.lowerRatio = 0.075;

	V3::Demographic demo2;
	demo2.culture = "cul2";
	demo2.religion = "rel2";
	demo2.upperRatio = 0.1;
	demo2.middleRatio = 0.1;
	demo2.lowerRatio = 0.5;

	subState.setDemographics({demo1, demo2});

	subState.generatePops(1000, 0);

	const auto& pops = subState.getSubStatePops().getPops();
	ASSERT_EQ(2, pops.size());
	const auto& pop1 = pops[0];
	const auto& pop2 = pops[1];

	EXPECT_EQ("cul1", pop1.getCulture());
	EXPECT_EQ(300, pop1.getSize());

	EXPECT_EQ("cul2", pop2.getCulture());
	EXPECT_EQ(700, pop2.getSize()); // <- primary culture

	ASSERT_TRUE(subState.getPrimaryCulture());
	EXPECT_EQ("cul2", *subState.getPrimaryCulture());
}

TEST(V3World_SubStateTests, SubStateGeneratesNoPopsForZeroPopsize)
{
	V3::SubState subState;
	V3::Demographic demo1;
	demo1.culture = "cul1";
	demo1.religion = "rel1";
	demo1.upperRatio = 0.15;
	demo1.middleRatio = 0.075;
	demo1.lowerRatio = 0.075;

	V3::Demographic demo2;
	demo2.culture = "cul2";
	demo2.religion = "rel2";
	demo2.upperRatio = 0.1;
	demo2.middleRatio = 0.1;
	demo2.lowerRatio = 0.5;

	subState.setDemographics({demo1, demo2});

	subState.generatePops(0, 0);

	const auto& pops = subState.getSubStatePops().getPops();
	ASSERT_EQ(0, pops.size());

	ASSERT_FALSE(subState.getPrimaryCulture());
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

	EXPECT_DOUBLE_EQ(0.5 / 3, substate.getTerrainFrequency("desert"));
	EXPECT_DOUBLE_EQ(2.0 / 3, substate.getTerrainFrequency("plains"));
	EXPECT_DOUBLE_EQ(0.5 / 3, substate.getTerrainFrequency("coastal"));
}

TEST(V3World_SubStateTests, InfrastructureCalculationIsolateStateModifieres)
{
	auto country = std::make_shared<V3::Country>();
	auto state = std::make_shared<V3::State>();
	auto substate = V3::SubState();

	substate.setOwner(country);
	substate.setHomeState(state);
	substate.setIncorporated(true);

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

	substate.calculateInfrastructure(V3::StateModifiers{{modifier0->getName(), modifier0}, {modifier1->getName(), modifier1}}, {}, 100000);

	EXPECT_DOUBLE_EQ(10.4, substate.getInfrastructure());
}

TEST(V3World_SubStateTests, InfrastructureCalculationIsolatePopFactor)
{
	// Setup Country
	auto country = std::make_shared<V3::Country>();
	auto state = std::make_shared<V3::State>();

	V3::ProcessedData data;
	data.techs.emplace("tech_1");
	country->setProcessedData(data);

	auto substate0 = V3::SubState();
	auto substate1 = V3::SubState();

	substate0.setOwner(country);
	substate0.setHomeState(state);
	substate0.setIncorporated(true);
	substate1.setOwner(country);
	substate1.setHomeState(state);
	substate1.setIncorporated(true);

	V3::Pop pop0;
	V3::Pop pop1;
	pop0.setSize(10000);
	pop1.setSize(10000000);

	substate0.addPop(pop0);
	substate1.addPop(pop1);


	// Setup Loader
	V3::TechLoader techLoader;
	techLoader.loadTechs(modFS);

	substate0.calculateInfrastructure({}, techLoader.getTechs(), 10000);
	substate1.calculateInfrastructure({}, techLoader.getTechs(), 10000);

	EXPECT_DOUBLE_EQ(3.2, substate0.getInfrastructure());
	EXPECT_DOUBLE_EQ(43, substate1.getInfrastructure());
}


TEST(V3World_SubStateTests, InfrastructureCalculationFactorsFromPrivateVariables)
{
	auto country = std::make_shared<V3::Country>();
	auto state = std::make_shared<V3::State>();
	auto substate = V3::SubState();

	substate.setOwner(country);
	substate.setHomeState(state);
	substate.setIncorporated(true);

	substate.calculateInfrastructure(V3::StateModifiers{}, {}, 100000);
	EXPECT_DOUBLE_EQ(3, substate.getInfrastructure());

	substate.setMarketCapital();
	substate.calculateInfrastructure(V3::StateModifiers{}, {}, 100000);
	EXPECT_DOUBLE_EQ(3.75, substate.getInfrastructure());

	substate.setIncorporated(false);
	substate.calculateInfrastructure(V3::StateModifiers{}, {}, 100000);
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

	substate.calculateInfrastructure(V3::StateModifiers{{modifier0->getName(), modifier0}, {modifier1->getName(), modifier1}}, {}, 100000);

	EXPECT_DOUBLE_EQ(0.0, substate.getInfrastructure());
}

TEST(V3World_SubStateTests, AddBuilding)
{
	auto substate = V3::SubState();
	V3::Building templateBuilding;
	templateBuilding.setName("port");
	templateBuilding.setLevel(1);

	const auto building1 = std::make_shared<V3::Building>(templateBuilding);
	const auto building2 = std::make_shared<V3::Building>(templateBuilding);

	building2->setLevel(2);

	substate.addBuilding(building1);
	substate.addBuilding(building2);

	EXPECT_EQ("port", substate.getBuildings()[0]->getName());
	EXPECT_EQ(1, substate.getBuildings()[0]->getLevel());
	EXPECT_EQ("port", substate.getBuildings()[1]->getName());
	EXPECT_EQ(2, substate.getBuildings()[1]->getLevel());
}