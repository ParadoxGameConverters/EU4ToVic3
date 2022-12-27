#include "EconomyManager/Building/ProductionMethods/ProductionMethod.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_ProductionMethodTests, ProductionMethodLoadsPMData)
{
	std::stringstream input;


	input << "\tunlocking_technologies = { fire }\n";
	input << "\tcountry_modifiers = { workforce_scaled = { country_bureaucracy_add = 35 } }\n";
	input << "\tbuilding_modifiers = { level_scaled = { building_employment_clerks_add = 4000 } }\n";
	input << "\tbuilding_modifiers = { level_scaled = { building_employment_mounties_add = 50 } }\n";

	V3::ProductionMethod PM;
	PM.loadProductionMethod(input);

	EXPECT_THAT(PM.getUnlockingTechs(), testing::UnorderedElementsAre("fire"));
	EXPECT_EQ(35, PM.getBureaucracy());
	EXPECT_THAT(PM.getEmployment(), testing::UnorderedElementsAre(std::make_pair("clerks", 4000), std::make_pair("mounties", 50)));
}