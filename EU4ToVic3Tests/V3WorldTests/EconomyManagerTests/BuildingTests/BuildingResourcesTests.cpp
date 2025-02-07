#include "EconomyManager/Building/BuildingResources.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
std::tuple<std::map<std::string, V3::ProductionMethodGroup>, std::map<std::string, V3::ProductionMethod>> loadPMData()
{
	std::stringstream inputPMGroupOne;
	std::stringstream inputPMGroupTwo;

	inputPMGroupOne << "production_methods = { a c }\n";
	inputPMGroupTwo << "production_methods = { b }\n";

	std::stringstream inputPMA;
	std::stringstream inputPMB;
	std::stringstream inputPMC;

	inputPMA << "building_modifiers = {\n";
	inputPMA << "workforce_scaled = { goods_input_wood_add = 10 goods_output_coal_add = 5 }\n";
	inputPMA << "level_scaled = { building_employment_laborers_add = 4500 building_employment_machinists_add = 500 }\n";
	inputPMA << "}\n";

	inputPMB << "building_modifiers = {\n";
	inputPMB << "workforce_scaled = { goods_input_wood_add = -5 goods_input_iron_add = 5 }\n";
	inputPMB << "level_scaled = { building_employment_laborers_add = -1000 }\n";
	inputPMB << "}\n";

	inputPMC << "building_modifiers = {\n";
	inputPMC << "workforce_scaled = { goods_input_wood_add = 20 goods_output_coal_add = 15 }\n";
	inputPMC << "level_scaled = { building_employment_laborers_add = 4000 building_employment_shopkeepers_add = 500 }\n";
	inputPMC << "}\n";

	V3::ProductionMethodGroup one, two;
	one.loadProductionMethodGroup(inputPMGroupOne);
	two.loadProductionMethodGroup(inputPMGroupTwo);

	V3::ProductionMethod a, b, c;
	a.loadProductionMethod(inputPMA);
	b.loadProductionMethod(inputPMB);
	c.loadProductionMethod(inputPMC);

	return {{{"one", one}, {"two", two}}, {{"a", a}, {"b", b}, {"c", c}}};
}
} // namespace
TEST(V3World_BuildingResources, DefaultsDefaultToDefaults)
{
	V3::BuildingResources buildingResources;

	EXPECT_TRUE(buildingResources.getJobs().empty());
	EXPECT_TRUE(buildingResources.getInputs().empty());
	EXPECT_TRUE(buildingResources.getOutputs().empty());
}

TEST(V3World_BuildingResources, NoRuleLoadsDefaultPMResources)
{
	const auto& [PMGroups, PMs] = loadPMData();

	V3::BuildingResources buildingResources;
	buildingResources.evaluateResources({"one", "two"}, {}, PMs, PMGroups);

	EXPECT_THAT(buildingResources.getJobs(), testing::UnorderedElementsAre(testing::Pair("laborers", 3500), testing::Pair("machinists", 500)));
	EXPECT_THAT(buildingResources.getInputs(), testing::UnorderedElementsAre(testing::Pair("wood", 5), testing::Pair("iron", 5)));
	EXPECT_THAT(buildingResources.getOutputs(), testing::UnorderedElementsAre(testing::Pair("coal", 5)));
}

TEST(V3World_BuildingResources, MaxRuleLoadsAdvancedPMResources)
{
	const auto& [PMGroups, PMs] = loadPMData();

	V3::BuildingResources buildingResources;
	buildingResources.evaluateResources({"one", "two"}, {{"one", {1, 1.0}}}, PMs, PMGroups);

	EXPECT_THAT(buildingResources.getJobs(), testing::UnorderedElementsAre(testing::Pair("laborers", 3000), testing::Pair("shopkeepers", 500)));
	EXPECT_THAT(buildingResources.getInputs(), testing::UnorderedElementsAre(testing::Pair("wood", 15), testing::Pair("iron", 5)));
	EXPECT_THAT(buildingResources.getOutputs(), testing::UnorderedElementsAre(testing::Pair("coal", 15)));
}

TEST(V3World_BuildingResources, MixedRuleLoadsMixedPMResources)
{
	const auto& [PMGroups, PMs] = loadPMData();

	V3::BuildingResources buildingResources;
	buildingResources.evaluateResources({"one", "two"}, {{"one", {1, 0.6}}}, PMs, PMGroups);

	EXPECT_THAT(buildingResources.getJobs(),
		 testing::UnorderedElementsAre(testing::Pair("laborers", 3200), testing::Pair("shopkeepers", 300), testing::Pair("machinists", 200)));
	EXPECT_THAT(buildingResources.getInputs(), testing::UnorderedElementsAre(testing::Pair("wood", 11), testing::Pair("iron", 5)));
	EXPECT_THAT(buildingResources.getOutputs(), testing::UnorderedElementsAre(testing::Pair("coal", 11)));
}