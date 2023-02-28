#include "Loaders/TechLoader/TechEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_TechEntryTests, DefaultsDefaultToDefaults)
{
	const V3::TechEntry entry;
	const auto& tech = entry.getTech();

	EXPECT_EQ(0, tech.infrastructureMax);
	EXPECT_DOUBLE_EQ(0.0, tech.infrastructureMult);
}

TEST(V3World_TechEntryTests, TechCanBeLoaded)
{
	std::stringstream input;
	input << "modifier = {\n ";
	input << "\tstate_infrastructure_from_population_add = 0.15\n";
	input << "\tstate_infrastructure_from_population_max_add = 5\n";
	input << "\tstate_building_port_max_level_add = 2\n";
	input << "\tstate_building_naval_base_max_level_add = 20\n";
	input << "\tbuilding_economy_of_scale_level_cap_add = 15\n";
	input << "}\n ";
	const V3::TechEntry entry(input);
	const auto& tech = entry.getTech();

	EXPECT_DOUBLE_EQ(0.15, tech.infrastructureMult);
	EXPECT_EQ(5, tech.infrastructureMax);
	EXPECT_EQ(2, tech.maxBuildingLevels.at("building_port"));
	EXPECT_EQ(20, tech.maxBuildingLevels.at("building_naval_base"));
	EXPECT_EQ(15, tech.throughputMax);
}
