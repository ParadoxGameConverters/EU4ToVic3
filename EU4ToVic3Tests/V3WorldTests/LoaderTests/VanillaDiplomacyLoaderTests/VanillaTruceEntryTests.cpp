#include "Loaders/VanillaDiplomacyLoader/VanillaTruceEntry.h"
#include "gtest/gtest.h"

TEST(V3World_VanillaTruceEntryTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaTruceEntry entry;

	EXPECT_TRUE(entry.getSourceTag().empty());
	EXPECT_TRUE(entry.getTargetTag().empty());
	EXPECT_EQ(0, entry.getDuration());
}

TEST(V3World_VanillaTruceEntryTests, entryCanBeLoaded)
{
	std::stringstream input;
	input << "country = c:GBR months = 6\n";
	const V3::VanillaTruceEntry entry(input, "GRE");

	EXPECT_EQ("GRE", entry.getSourceTag());
	EXPECT_EQ("GBR", entry.getTargetTag());
	EXPECT_EQ(6, entry.getDuration());
}
