#include "PopManager/Pops/PopType.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_PopTypeTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;

	const V3::PopType popType(input);

	EXPECT_EQ("", popType.getType());
	EXPECT_DOUBLE_EQ(1, popType.getConsumptionRate());
	EXPECT_EQ(std::nullopt, popType.getDependentRatio());
	EXPECT_EQ(7, popType.getStartingQOL());
}

TEST(V3World_PopTypeTests, PopTypeLoadsPopTypeData)
{
	std::stringstream input;

	input << "start_quality_of_life = 5\n";
	input << "working_adult_ratio = 0.2\n";
	input << "consumption_mult = 0.05\n";

	V3::PopType popType(input);
	popType.setType("aristocratic_peasants");

	EXPECT_EQ("aristocratic_peasants", popType.getType());
	EXPECT_DOUBLE_EQ(0.05, popType.getConsumptionRate());
	EXPECT_DOUBLE_EQ(0.2, popType.getDependentRatio().value());
	EXPECT_EQ(5, popType.getStartingQOL());
}