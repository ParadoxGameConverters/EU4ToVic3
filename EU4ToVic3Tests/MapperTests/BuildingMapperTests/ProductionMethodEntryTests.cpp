#include "Mappers/BuildingMapper/ProductionMethodMapper/ProductionMethodEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ProductionMethodEntryTests, DefaultsDefaultToDefaults)
{
	const mappers::ProductionMethodEntry entry;
	const auto& rule = entry.getRule();

	EXPECT_TRUE(rule.pm.empty());
	EXPECT_DOUBLE_EQ(1, rule.percent);
	EXPECT_FALSE(rule.lawBound);
}

TEST(V3World_ProductionMethodEntryTests, PMCanBeLoaded)
{
	std::stringstream input;
	input << "name = pm_hardwood\n ";
	input << "percent = 0.33\n ";
	input << "law_bound = yes\n";

	const mappers::ProductionMethodEntry entry(input);
	const auto& rule = entry.getRule();

	EXPECT_EQ("pm_hardwood", rule.pm);
	EXPECT_DOUBLE_EQ(0.33, rule.percent);
	EXPECT_TRUE(rule.lawBound);
}