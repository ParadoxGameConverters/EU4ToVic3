#include "Loaders/VanillaStateLoader/VanillaStateEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_VanillaStateEntryTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaStateEntry entry;

	EXPECT_TRUE(entry.getHomelands().empty());
	EXPECT_TRUE(entry.getSubStates().empty());
}

TEST(V3World_VanillaStateEntryTests, StateEntryCanBeLoaded)
{
	std::stringstream input;
	input << "create_state= {\n ";
	input << "	country = c:USA\n ";
	input << "	owned_provinces = {\n";
	input << "		x5A4A79 \"x450FD3\" x7B3020\n";
	input << "	}\n";
	input << "	state_type = unincorporated\n";
	input << "}\n ";
	input << "create_state = {\n ";
	input << "	country = c:USB\n ";
	input << "	owned_provinces = {\n";
	input << "		x5A4A70 \"x450FD0\" x7B3000\n";
	input << "	}\n";
	input << "}\n ";
	input << "add_homeland = yankee\n";
	input << "add_homeland = dakota\n";
	const V3::VanillaStateEntry entry(input);

	EXPECT_THAT(entry.getHomelands(), testing::UnorderedElementsAre("yankee", "dakota"));
	ASSERT_EQ(2, entry.getSubStates().size());

	const auto& sub1 = entry.getSubStates()[0];
	const auto& sub2 = entry.getSubStates()[1];
	EXPECT_EQ("USA", sub1.getOwnerTag());
	EXPECT_EQ("USB", sub2.getOwnerTag());
}
