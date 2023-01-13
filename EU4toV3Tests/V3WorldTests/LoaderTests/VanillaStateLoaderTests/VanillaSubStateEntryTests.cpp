#include "Loaders/VanillaStateLoader/VanillaSubStateEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_VanillaSubStateEntryTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaSubStateEntry entry;

	EXPECT_TRUE(entry.getOwnerTag().empty());
	EXPECT_TRUE(entry.getSubStateTypes().empty());
	EXPECT_TRUE(entry.getProvinces().empty());
}

TEST(V3World_VanillaSubStateEntryTests, entryCanBeLoaded)
{
	std::stringstream input;
	input << "country = c:USA\n ";
	input << "owned_provinces = {\n";
	input << "	x5A4A79 \"x450FD3\" x7B3020\n";
	input << "}\n";
	input << "state_type = unincorporated\n";
	input << "state_type = treaty_port\n";
	const V3::VanillaSubStateEntry entry(input);

	EXPECT_EQ("USA", entry.getOwnerTag());
	EXPECT_THAT(entry.getSubStateTypes(), testing::UnorderedElementsAre("unincorporated", "treaty_port"));
	EXPECT_THAT(entry.getProvinces(), testing::UnorderedElementsAre("x5A4A79", "x450FD3", "x7B3020"));
}

TEST(V3World_VanillaSubStateEntryTests, oddSubStateOwnersThrowWarning)
{
	std::stringstream input;
	input << "country = USA\n ";
	input << "owned_provinces = {\n";
	input << "	x5A4A79 \"x450FD3\" x7B3020\n";
	input << "}\n";
	input << "state_type = unincorporated\n";

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	const V3::VanillaSubStateEntry entry(input);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Unrecognized substate country for USA!)"));
	std::cout.rdbuf(cout_buffer);

	EXPECT_EQ("USA", entry.getOwnerTag());
	EXPECT_THAT(entry.getSubStateTypes(), testing::UnorderedElementsAre("unincorporated"));
	EXPECT_THAT(entry.getProvinces(), testing::UnorderedElementsAre("x5A4A79", "x450FD3", "x7B3020"));
}
