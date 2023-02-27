#include "Loaders/VanillaDiplomacyLoader/VanillaRelationEntry.h"
#include "gtest/gtest.h"

TEST(V3World_VanillaRelationEntryTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaRelationEntry entry;

	EXPECT_TRUE(entry.getSourceTag().empty());
	EXPECT_TRUE(entry.getTargetTag().empty());
	EXPECT_EQ(0, entry.getRelationValue());
}

TEST(V3World_VanillaRelationEntryTests, entryCanBeLoaded)
{
	std::stringstream input;
	input << "country = c:BUR value = -30\n";
	const V3::VanillaRelationEntry entry(input, "BIC");

	EXPECT_EQ("BIC", entry.getSourceTag());
	EXPECT_EQ("BUR", entry.getTargetTag());
	EXPECT_EQ(-30, entry.getRelationValue());
}
