#include "Loaders/PopLoader/PopEntry.h"
#include "gtest/gtest.h"

TEST(V3World_PopEntryTests, DefaultsDefaultToDefaults)
{
	const V3::PopEntry popEntry;

	EXPECT_TRUE(popEntry.getPop().getCulture().empty());
	EXPECT_TRUE(popEntry.getPop().getReligion().empty());
	EXPECT_TRUE(popEntry.getPop().getType().empty());
	EXPECT_FALSE(popEntry.getPop().isMinority());
	EXPECT_EQ(0, popEntry.getPop().getSize());
}

TEST(V3World_PopEntryTests, PopCanBeLoaded)
{
	V3::PopEntry popEntry;
	std::stringstream input;
	input << "culture = cul\n";
	input << "religion = rel\n ";
	input << "pop_type = typ\n";
	input << "size = 10\n ";
	popEntry.loadPop(input);

	EXPECT_EQ("cul", popEntry.getPop().getCulture());
	EXPECT_EQ("rel", popEntry.getPop().getReligion());
	EXPECT_EQ("typ", popEntry.getPop().getType());
	EXPECT_FALSE(popEntry.getPop().isMinority());
	EXPECT_EQ(10, popEntry.getPop().getSize());
}
