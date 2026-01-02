#include "ClayManager/ClayManager.h"
#include "ClayManager/State/Province.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game", {});
}

TEST(V3World_StateTests, nameCanBeSetAndRetrieved)
{
	V3::State state;

	EXPECT_EQ(std::string(), state.getName());
	state.setStateName("test_name");
	EXPECT_EQ("test_name", state.getName());
}

TEST(V3World_StateTests, stateSubsistenceBuildingLoads)
{
	std::stringstream input;
	input << "id = 3002\n";
	input << "subsistence_building = building_subsistence_farm\n ";
	V3::State state;
	state.loadState(input);

	EXPECT_EQ(state.getSubsistenceBuilding(), "building_subsistence_farm");
}

TEST(V3World_StateTests, stateCanInitializeProvinces)
{
	std::stringstream input;
	input << "id = 3002\n";
	input << "provinces = { \"x112233\" \"x445566\"}\n ";
	V3::State state;
	state.loadState(input);

	EXPECT_EQ(2, state.getProvinces().size());
	EXPECT_TRUE(state.containsProvince("x112233"));
	EXPECT_TRUE(state.containsProvince("x445566"));
	EXPECT_FALSE(state.containsProvince("x778899"));
}

TEST(V3World_StateTests, provinceNamesAreUppercased)
{
	std::stringstream input;
	input << "provinces = { \"xabcdef\" }\n ";
	V3::State state;
	state.loadState(input);

	EXPECT_TRUE(state.containsProvince("xABCDEF"));
}

TEST(V3World_StateTests, provincesCanBeRequisitioned)
{
	std::stringstream input;
	input << "provinces = { \"xabcdef\" }\n ";
	V3::State state;
	state.loadState(input);

	const auto& province = state.getProvince("xABCDEF");

	EXPECT_EQ("xABCDEF", province->getName());
}

TEST(V3World_StateTests, InvalidProvinceRequestReturnsNullptr)
{
	std::stringstream input;
	input << "provinces = { \"xabcdef\" }\n ";
	V3::State state;
	state.loadState(input);

	const auto& province = state.getProvince("x111111");

	EXPECT_EQ(nullptr, province);
}

TEST(V3World_StateTests, invalidProvinceLengthNamesAreUppercasedAndObjected)
{
	std::stringstream input;
	input << "provinces = { \"x12345\" \"x1234567\" \"0x123456\" }\n ";
	V3::State state;

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	state.loadState(input);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Loading province X12345 in unknown format!)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Loading province X1234567 in unknown format!)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Loading province 0X123456 in unknown format!)"));

	std::cout.rdbuf(cout_buffer);
}


TEST(V3World_StateTests, primeProvincesGetFlagged)
{
	std::stringstream input;
	input << "id = 3002\n";
	input << "provinces = { \"x112233\" \"x445566\"}\n ";
	input << "prime_land = { \"x445566\"}\n ";
	V3::State state;
	state.loadState(input);

	const auto& province1 = state.getProvinces().at("x112233");
	const auto& province2 = state.getProvinces().at("x445566");

	EXPECT_FALSE(province1->isPrime());
	EXPECT_TRUE(province2->isPrime());
}


TEST(V3World_StateTests, primeProvinceNamesAreUppercasedBeforeMatching)
{
	std::stringstream input;
	input << "provinces = { \"xABCDEF\" }\n ";
	input << "prime_land = { \"xabcdef\"}\n ";
	V3::State state;
	state.loadState(input);

	const auto& province1 = state.getProvinces().at("xABCDEF");
	EXPECT_TRUE(province1->isPrime());
}

TEST(V3World_StateTests, invalidPrimeProvinceLengthNamesAreUppercasedAndObjected)
{
	std::stringstream input;
	input << "provinces = { \"x12345\" \"x1234567\" \"0x123456\" }\n ";
	input << "prime_land = { \"x12345\" \"x1234567\" \"0x123456\" }\n ";
	V3::State state;

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	state.loadState(input);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Encountered prime province X12345 in unknown format!)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Encountered prime province X1234567 in unknown format!)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Encountered prime province 0X123456 in unknown format!)"));

	std::cout.rdbuf(cout_buffer);
}

TEST(V3World_StateTests, excessPrimeProvincesAreSimplyIgnored)
{
	std::stringstream input;
	input << "provinces = { \"x123456\" }\n ";
	input << "prime_land = { \"x123456\" \"x123457\" \"x123458\" }\n ";
	V3::State state;
	state.setStateName("test");

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	state.loadState(input);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Prime province x123457 isn't defined in the state test! Ignoring.)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Prime province x123458 isn't defined in the state test! Ignoring.)"));

	std::cout.rdbuf(cout_buffer);

	const auto& province1 = state.getProvinces().at("x123456");
	EXPECT_TRUE(province1->isPrime());

	EXPECT_FALSE(state.getProvinces().contains("x123457"));
	EXPECT_FALSE(state.getProvinces().contains("x123458"));
}

TEST(V3World_StateTests, impassableProvincesGetFlagged)
{
	std::stringstream input;
	input << "id = 3002\n";
	input << "provinces = { \"x112233\" \"x445566\"}\n ";
	input << "impassable = { \"x445566\"}\n ";
	V3::State state;
	state.loadState(input);

	const auto& province1 = state.getProvinces().at("x112233");
	const auto& province2 = state.getProvinces().at("x445566");

	EXPECT_FALSE(province1->isImpassable());
	EXPECT_TRUE(province2->isImpassable());
}


TEST(V3World_StateTests, impassableProvinceNamesAreUppercasedBeforeMatching)
{
	std::stringstream input;
	input << "provinces = { \"xABCDEF\" }\n ";
	input << "impassable = { \"xabcdef\"}\n ";
	V3::State state;
	state.loadState(input);

	const auto& province1 = state.getProvinces().at("xABCDEF");
	EXPECT_TRUE(province1->isImpassable());
}

TEST(V3World_StateTests, invalidImpassableProvinceLengthNamesAreUppercasedAndObjected)
{
	std::stringstream input;
	input << "provinces = { \"x12345\" \"x1234567\" \"0x123456\" }\n ";
	input << "impassable = { \"x12345\" \"x1234567\" \"0x123456\" }\n ";
	V3::State state;

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	state.loadState(input);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Encountered impassable province X12345 in unknown format!)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Encountered impassable province X1234567 in unknown format!)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Encountered impassable province 0X123456 in unknown format!)"));

	std::cout.rdbuf(cout_buffer);
}

TEST(V3World_StateTests, excessImpassableProvincesAreSimplyIgnored)
{
	std::stringstream input;
	input << "provinces = { \"x123456\" }\n ";
	input << "impassable = { \"x123456\" \"x123457\" \"x123458\" }\n ";
	V3::State state;
	state.setStateName("test");

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	state.loadState(input);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Impassable province x123457 isn't defined in the state test! Ignoring.)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Impassable province x123458 isn't defined in the state test! Ignoring.)"));

	std::cout.rdbuf(cout_buffer);

	const auto& province1 = state.getProvinces().at("x123456");
	EXPECT_TRUE(province1->isImpassable());

	EXPECT_FALSE(state.getProvinces().contains("x123457"));
	EXPECT_FALSE(state.getProvinces().contains("x123458"));
}


TEST(V3World_StateTests, arableLandValueSet)
{
	std::stringstream input;
	input << "id = 3002\n";
	input << "provinces = { \"x112233\" }\n ";
	input << "arable_land = 36\n ";
	V3::State state;

	EXPECT_EQ(state.getCappedResources().find("bg_agriculture"), state.getCappedResources().end());
	state.loadState(input);
	EXPECT_EQ(state.getCappedResources().at("bg_agriculture"), 36);
}


TEST(V3World_StateTests, cappedResourcesValueSet)
{
	std::stringstream input;
	input << "id = 3002\n";
	input << "provinces = { \"x112233\" }\n ";
	input << "capped_resources = { iron = 40 lead = 18 }\n ";
	V3::State state;

	EXPECT_EQ(state.getCappedResources().find("iron"), state.getCappedResources().end());
	EXPECT_EQ(state.getCappedResources().find("lead"), state.getCappedResources().end());
	state.loadState(input);
	EXPECT_EQ(state.getCappedResources().at("iron"), 40);
	EXPECT_EQ(state.getCappedResources().at("lead"), 18);
}

TEST(V3World_StateTests, navalExitFlagsStateAsCoastal)
{
	std::stringstream input;
	input << "id = 3002\n";
	input << "provinces = { \"x112233\" \"x445566\"}\n ";
	input << "impassable = { \"x445566\"}\n ";
	input << "naval_exit_id = 3026\n ";
	V3::State state;

	EXPECT_FALSE(state.isCoastal());
	state.loadState(input);
	EXPECT_TRUE(state.isCoastal());
}


TEST(V3World_StateTests, landshareAccountsForPrimeAndImpassable)
{
	std::stringstream input;
	input << "id = 3002\n";
	input << "provinces = { \"x112233\" \"x112234\" \"x112235\" \"x112236\" \"x112237\" \"x445566\"}\n ";
	input << "prime_land = {\"x112234\"}\n ";
	input << "impassable = { \"x445566\"}\n ";
	V3::State state;

	state.loadState(input);

	const auto pm = state.getProvinces();

	const auto zeroth = std::make_shared<V3::SubState>();
	const auto first = std::make_shared<V3::SubState>();
	const auto second = std::make_shared<V3::SubState>();
	zeroth->setProvinces(std::map<std::string, std::shared_ptr<V3::Province>>{{"x112233", pm.at("x112233")}, {"x112234", pm.at("x112234")}});
	first->setProvinces(std::map<std::string, std::shared_ptr<V3::Province>>{{"x112235", pm.at("x112235")}, {"x112236", pm.at("x112236")}});
	second->setProvinces(std::map<std::string, std::shared_ptr<V3::Province>>{{"x112237", pm.at("x112237")}, {"x445566", pm.at("x445566")}});

	state.addSubState(zeroth);
	state.addSubState(first);
	state.addSubState(second);

	state.distributeLandshares(5);

	const auto& substates = state.getSubStates();
	EXPECT_DOUBLE_EQ(substates[0]->getLandshare(), 6.0 / 9);
	EXPECT_DOUBLE_EQ(substates[1]->getLandshare(), 2.0 / 9);
	EXPECT_DOUBLE_EQ(substates[2]->getLandshare(), 1.0 / 9);
}

TEST(V3World_StateTests, distributeResourcesTruncatesDoubles)
{
	std::stringstream input;
	input << "id = 3002\n";
	input << "provinces = { \"x112233\" \"x112234\" \"x112235\" \"x112236\" \"x112237\" \"x445566\"}\n ";
	input << "prime_land = {\"x112234\"}\n ";
	input << "impassable = { \"x445566\"}\n ";
	input << "arable_land = 36\n ";
	input << "arable_resources = { rice wheat }\n";
	input << "capped_resources = { iron = 53 }\n ";
	V3::State state;

	state.loadState(input);

	const auto zeroth = std::make_shared<V3::SubState>();
	const auto first = std::make_shared<V3::SubState>();
	const auto second = std::make_shared<V3::SubState>();

	zeroth->setLandshare(5.0 / 10);
	first->setLandshare(3.0 / 10);
	second->setLandshare(3.8 / 9);

	state.addSubState(zeroth);
	state.addSubState(first);
	state.addSubState(second);

	state.distributeResources();

	const auto& substates = state.getSubStates();
	EXPECT_EQ(substates[0]->getResource("bg_agriculture"), 18);
	EXPECT_EQ(substates[1]->getResource("bg_agriculture"), 10);
	EXPECT_EQ(substates[2]->getResource("bg_agriculture"), 15);

	EXPECT_EQ(substates[0]->getResource("iron"), 26);
	EXPECT_EQ(substates[1]->getResource("iron"), 15);
	EXPECT_EQ(substates[2]->getResource("iron"), 22);
}

TEST(V3World_StateTests, StateSeasCanBePinged)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.loadTerrainsIntoProvinces(modFS);

	const auto& state1 = clayManager.getStates().at("STATE_TEST_OCEAN1");
	const auto& state2 = clayManager.getStates().at("STATE_TEST_LAND1");

	EXPECT_TRUE(state1->isSea());
	EXPECT_FALSE(state2->isSea());
}

TEST(V3World_StateTests, StateLakesCanBePinged)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.loadTerrainsIntoProvinces(modFS);

	const auto& state1 = clayManager.getStates().at("STATE_TEST_OCEAN1");
	const auto& state2 = clayManager.getStates().at("STATE_TEST_LAKE");
	const auto& state3 = clayManager.getStates().at("STATE_TEST_LAND1");

	EXPECT_FALSE(state1->isLake());
	EXPECT_TRUE(state2->isLake());
	EXPECT_FALSE(state3->isLake());
}

TEST(V3World_StateTests, stateCanDetectUnassignedProvinces)
{
	std::stringstream input;
	input << "provinces = { x000001 x000002 x000003 }\n ";
	V3::State state;
	state.loadState(input);

	// assign some provinces to substates.
	const auto prov1 = state.getProvinces().at("x000001");
	const auto prov2 = state.getProvinces().at("x000002");

	const auto sub1 = std::make_shared<V3::SubState>();
	sub1->setProvinces({std::pair("x000001", prov1)});

	const auto sub2 = std::make_shared<V3::SubState>();
	sub2->setProvinces({std::pair("x000002", prov2)});

	state.addSubState(sub1);
	state.addSubState(sub2);

	EXPECT_TRUE(state.hasUnassignedProvinces());
}

TEST(V3World_StateTests, stateCanReturnUnassignedProvinces)
{
	std::stringstream input;
	input << "provinces = { x000001 x000002 x000003 x000004 }\n ";
	V3::State state;
	state.loadState(input);

	// assign some provinces to substates.
	const auto prov1 = state.getProvinces().at("x000001");
	const auto prov2 = state.getProvinces().at("x000002");

	const auto sub1 = std::make_shared<V3::SubState>();
	sub1->setProvinces({std::pair("x000001", prov1)});

	const auto sub2 = std::make_shared<V3::SubState>();
	sub2->setProvinces({std::pair("x000002", prov2)});

	state.addSubState(sub1);
	state.addSubState(sub2);

	ASSERT_EQ(2, state.getUnassignedProvinces().size());
	EXPECT_EQ("x000003", state.getUnassignedProvinces().at("x000003")->getName());
	EXPECT_EQ("x000004", state.getUnassignedProvinces().at("x000004")->getName());
}

TEST(V3World_StateTests, stateCanSumItsPops)
{
	V3::State state;
	const auto sub1 = std::make_shared<V3::SubState>();
	const auto sub2 = std::make_shared<V3::SubState>();
	V3::SubStatePops somePops;
	somePops.setPops({V3::Pop("a", "A", "", 10), V3::Pop("a", "A", "", 30)});
	sub1->setSubStatePops(somePops);
	state.addSubState(sub1);
	state.addSubState(sub2);

	EXPECT_EQ(40, state.getStatePopCount());
}

TEST(V3World_StateTests, stateCanSumItsWeights)
{
	V3::State state;
	const auto sub1 = std::make_shared<V3::SubState>();
	const auto sub2 = std::make_shared<V3::SubState>();
	sub1->setWeight(256.0);
	state.addSubState(sub1);
	state.addSubState(sub2);

	EXPECT_EQ(256.0, state.getTotalSubStateWeight());
}
