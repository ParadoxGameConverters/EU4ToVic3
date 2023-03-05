#include "EconomyManager/Building/ProductionMethods/ProductionMethodGroup.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_ProductionMethodGroupsTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;

	V3::ProductionMethodGroup PMGroup;
	PMGroup.loadProductionMethodGroup(input);

	EXPECT_TRUE(PMGroup.getPMs().empty());
}

TEST(V3World_ProductionMethodGroupsTests, ProductionMethodGroupLoadsPMList)
{
	std::stringstream input;
	input << "\tai_selection = most_productive\n";
	input << "\tproduction_methods = { pm_one pm_two }\n";

	V3::ProductionMethodGroup PMGroup;
	PMGroup.loadProductionMethodGroup(input);

	EXPECT_THAT(PMGroup.getPMs(), testing::UnorderedElementsAre("pm_one", "pm_two"));
}