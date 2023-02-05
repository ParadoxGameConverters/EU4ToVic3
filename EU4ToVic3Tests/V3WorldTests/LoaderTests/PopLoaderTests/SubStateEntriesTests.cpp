#include "Loaders/PopLoader/SubStateEntries.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_SubStateEntriesTests, DefaultsDefaultToDefaults)
{
	V3::SubStateEntries sse;

	EXPECT_TRUE(sse.getSubStatePops().empty());
}

TEST(V3World_SubStateEntriesTests, SubStatePopsCanBeLoaded)
{
	V3::SubStateEntries sse;
	std::stringstream input;
	input << "region_state:AAA = {\n";
	input << "	create_pop = {\n";
	input << "		culture = cul\n";
	input << "		religion = rel\n ";
	input << "		size = 10\n ";
	input << "	}\n";
	input << "	create_pop = {\n";
	input << "		culture = cul2\n";
	input << "		religion = rel2\n ";
	input << "		size = 20\n ";
	input << "	}\n";
	input << "}\n";
	input << "region_state:BBB = {\n";
	input << "	create_pop = {\n";
	input << "		culture = cul3\n";
	input << "		religion = rel3\n ";
	input << "		size = 100\n ";
	input << "	}\n";
	input << "}\n";
	sse.loadSubStates(input);

	ASSERT_EQ(2, sse.getSubStatePops().size());
	const auto& ssp1 = sse.getSubStatePops()[0];
	const auto& ssp2 = sse.getSubStatePops()[1];

	EXPECT_EQ("AAA", ssp1.getTag());
	EXPECT_EQ("BBB", ssp2.getTag());

	EXPECT_EQ(30, ssp1.getPopCount());
	EXPECT_EQ(100, ssp2.getPopCount());
}

TEST(V3World_SubStateEntriesTests, SubStateEntiresWarnsForCrapOwnerTagAndClearsIt)
{
	V3::SubStateEntries sse;
	std::stringstream input;
	input << "region_state:A = {\n";
	input << "	create_pop = {\n";
	input << "		culture = cul2\n";
	input << "		religion = rel2\n ";
	input << "		size = 20\n ";
	input << "	}\n";
	input << "}\n";

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	sse.loadSubStates(input);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Incoming pops within substate region_state:A have no parsable TAG. Filing pops as unclaimed.)"));
	std::cout.rdbuf(cout_buffer);


	ASSERT_EQ(1, sse.getSubStatePops().size());
	const auto& ssp1 = sse.getSubStatePops()[0];

	EXPECT_TRUE(ssp1.getTag().empty());
	EXPECT_EQ(20, ssp1.getPopCount());
}
