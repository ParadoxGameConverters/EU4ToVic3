#include "ClayManager/StateLoader/State.h"
#include "gtest/gtest.h"

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
