#include "Loaders/VanillaStateLoader/VanillaStateEntries.h"
#include "gtest/gtest.h"

TEST(V3World_VanillaStateEntriesTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaStateEntries entries;

	EXPECT_TRUE(entries.getStates().empty());
}

TEST(V3World_VanillaStateEntriesTests, StateEntriesCanBeLoaded)
{
	std::stringstream input;
	input << "S:STATE_A = {\n ";
	input << "	create_state= {\n ";
	input << "		country = c:USA\n ";
	input << "		owned_provinces = {\n";
	input << "			x5A4A79 \"x450FD3\" x7B3020\n";
	input << "		}\n";
	input << "		state_type = unincorporated\n";
	input << "	}\n ";
	input << "	add_homeland = dakota\n";
	input << "}\n";
	input << "s:STATE_B = {\n ";
	input << "	create_state = {\n ";
	input << "		country = c:USB\n ";
	input << "		owned_provinces = {\n";
	input << "			x5A4A70 \"x450FD0\" x7B3000\n";
	input << "		}\n";
	input << "	}\n ";
	input << "	add_homeland = yankee\n";
	input << "}\n";
	const V3::VanillaStateEntries entries(input);

	ASSERT_EQ(2, entries.getStates().size());
	const auto& state1 = entries.getStates().at("STATE_A");
	const auto& state2 = entries.getStates().at("STATE_B");

	EXPECT_EQ("dakota", *state1.getHomelands().begin());
	EXPECT_EQ("yankee", *state2.getHomelands().begin());
}
