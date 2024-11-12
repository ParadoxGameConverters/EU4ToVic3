#include "EconomyManager/Building/ProductionMethods/ProductionMethod.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_ProductionMethodTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;

	V3::ProductionMethod PM;
	PM.loadProductionMethod(input);

	EXPECT_TRUE(PM.getUnlockingTechs().empty());
	EXPECT_EQ(0, PM.getBureaucracy());
	EXPECT_TRUE(PM.getEmployment().empty());
	EXPECT_TRUE(PM.getName().empty());
}

TEST(V3World_ProductionMethodTests, ProductionMethodLoadsPMData)
{
	std::stringstream input;

	input << "\tunlocking_technologies = { fire }\n";
	input << "\tunlocking_laws = { unions }\n";
	input << "\tdisallowing_laws = { inheritance }\n";
	input << "\tcountry_modifiers = { workforce_scaled = { country_bureaucracy_add = 35 } }\n";
	input << "\tbuilding_modifiers = { level_scaled = { building_employment_clerks_add = 4000 } }\n";
	input << "\tbuilding_modifiers = { level_scaled = { building_employment_mounties_add = 50 } }\n";
	input << "\tbuilding_modifiers = {\n";
	input << "\tworkforce_scaled = {\n";
	input << "\tgoods_input_tools_add = 15\n";
	input << "\tgoods_input_oil_add = 5\n";
	input << "\tgoods_output_coal_add = 90\n";
	input << "\tgoods_output_grain_add = 10\n";
	input << "\tgoods_output_small_arms_add = 1\n";
	input << "\t}\n";
	input << "\t}\n";

	V3::ProductionMethod PM;
	PM.loadProductionMethod(input);

	EXPECT_THAT(PM.getUnlockingTechs(), testing::UnorderedElementsAre("fire"));
	EXPECT_THAT(PM.getUnlockingLaws(), testing::UnorderedElementsAre("unions"));
	EXPECT_THAT(PM.getBlockingLaws(), testing::UnorderedElementsAre("inheritance"));
	EXPECT_EQ(35, PM.getBureaucracy());
	EXPECT_THAT(PM.getEmployment(), testing::UnorderedElementsAre(std::make_pair("clerks", 4000), std::make_pair("mounties", 50)));
	EXPECT_THAT(PM.getInputs(), testing::UnorderedElementsAre(std::make_pair("tools", 15), std::make_pair("oil", 5)));
	EXPECT_THAT(PM.getOutputs(), testing::UnorderedElementsAre(std::make_pair("coal", 90), std::make_pair("grain", 10), std::make_pair("small_arms", 1)));
}

TEST(V3World_ProductionMethodTests, ProductionMethodGetTypeExtractsMiddleRegex)
{
	std::stringstream input;
	input << "\tbuilding_modifiers = {\n";
	input << "\tworkforce_scaled = {\n";
	input << "\tgoods_output_grain_add = 10\n";
	input << "\tgoods_input_small_arms_add = 1\n";
	input << "\t}\n";
	input << "\t}\n";

	V3::ProductionMethod PM;
	PM.loadProductionMethod(input);

	// Make sure keywords with _s in the name are captured in full, and not just the first part. E.g. "small" vs "small_arms"
	EXPECT_THAT(PM.getInputs(), testing::UnorderedElementsAre(std::make_pair("small_arms", 1)));
	EXPECT_THAT(PM.getOutputs(), testing::UnorderedElementsAre(std::make_pair("grain", 10)));
}