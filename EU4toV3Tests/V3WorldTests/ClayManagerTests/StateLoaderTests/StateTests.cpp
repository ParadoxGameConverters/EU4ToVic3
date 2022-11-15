#include "ClayManager/StateLoader/State.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_StateTests, nameCanBeSetAndRetrieved)
{
	V3::State state;

	EXPECT_EQ(std::string(), state.getName());
	state.setStateName("test_name");
	EXPECT_EQ("test_name", state.getName());
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

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	state.loadState(input);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Impassable province x123457 isn't defined in the state! Ignoring.)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Impassable province x123458 isn't defined in the state! Ignoring.)"));

	std::cout.rdbuf(cout_buffer);

	const auto& province1 = state.getProvinces().at("x123456");
	EXPECT_TRUE(province1->isImpassable());

	EXPECT_FALSE(state.getProvinces().contains("x123457"));
	EXPECT_FALSE(state.getProvinces().contains("x123458"));
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
