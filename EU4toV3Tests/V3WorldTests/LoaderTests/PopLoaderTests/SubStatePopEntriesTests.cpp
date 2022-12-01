#include "Loaders/PopLoader/SubStatePopEntries.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_SubStatePopEntriesTests, DefaultsDefaultToDefaults)
{
	const V3::SubStatePopEntries sspe;

	EXPECT_TRUE(sspe.getPops().empty());
}

TEST(V3World_SubStatePopEntriesTests, PopsCanBeLoaded)
{
	V3::SubStatePopEntries sspe;
	std::stringstream input;
	input << "create_pop = {\n";
	input << "	culture = cul\n";
	input << "	religion = rel\n ";
	input << "}\n";
	input << "create_pop = {\n";
	input << "	culture = cul2\n";
	input << "	religion = rel2\n ";
	input << "}\n";
	sspe.loadPops(input);

	EXPECT_THAT(sspe.getPops(), testing::UnorderedElementsAre(V3::Pop("cul", "rel", "", 0), V3::Pop("cul2", "rel2", "", 0)));
}
