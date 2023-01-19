#include "Loaders/PopLoader/StateEntries.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_StateEntriesTests, DefaultsDefaultToDefaults)
{
	V3::StateEntries se;

	EXPECT_TRUE(se.getStatePops().empty());
}

TEST(V3World_StateEntriesTests, StatePopsCanBeLoaded)
{
	V3::StateEntries se;
	std::stringstream input;
	input << "s:STATE_TEST1 = {\n";
	input << "	region_state:AAA = {\n";
	input << "		create_pop = {\n";
	input << "			culture = cul\n";
	input << "			religion = rel\n ";
	input << "			size = 10\n ";
	input << "		}\n";
	input << "		create_pop = {\n";
	input << "			culture = cul2\n";
	input << "			religion = rel2\n ";
	input << "			size = 20\n ";
	input << "		}\n";
	input << "	}\n";
	input << "	region_state:BBB = {\n";
	input << "		create_pop = {\n";
	input << "			culture = cul3\n";
	input << "			religion = rel3\n ";
	input << "			size = 100\n ";
	input << "		}\n";
	input << "	}\n";
	input << "}\n";
	input << "s:STATE_TEST2 = {\n";
	input << "	region_state:AAA = {\n";
	input << "		create_pop = {\n";
	input << "			culture = cul\n";
	input << "			religion = rel\n ";
	input << "			size = 1000\n ";
	input << "		}\n";
	input << "	}\n";
	input << "}\n";
	se.loadStates(input);

	ASSERT_EQ(2, se.getStatePops().size());

	const auto& sp1 = se.getStatePops().at("STATE_TEST1");
	const auto& sp2 = se.getStatePops().at("STATE_TEST2");

	EXPECT_EQ("STATE_TEST1", sp1.getStateName());
	EXPECT_EQ("STATE_TEST2", sp2.getStateName());

	EXPECT_EQ(130, sp1.getPopCount());
	EXPECT_EQ(1000, sp2.getPopCount());
}

TEST(V3World_StateEntriesTests, StateEntiresWarnsForCrapStateNameAndIgnoresIt)
{
	V3::StateEntries se;
	std::stringstream input;
	input << "s:A = {\n";
	input << "	region_state:AAA = {\n";
	input << "		create_pop = {\n";
	input << "			culture = cul\n";
	input << "			religion = rel\n ";
	input << "			size = 1000\n ";
	input << "		}\n";
	input << "	}\n";
	input << "}\n";

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	se.loadStates(input);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Incoming pops within state s:A have no parsable state. Deleting Pops!)"));
	std::cout.rdbuf(cout_buffer);

	EXPECT_TRUE(se.getStatePops().empty());
}
