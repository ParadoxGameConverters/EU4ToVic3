#include "BuildingMapper/ProductionMethodMapper/ProductionMethodMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ProductionMethodMappingTests, EmptyInputCreatesEmptyDefault)
{

	std::stringstream input;
	mappers::ProductionMethodMapping rules;
	rules.loadRules(input);

	EXPECT_TRUE(rules.getRules().empty());
}

TEST(Mappers_ProductionMethodMappingTests, RulesCanBeLoaded)
{

	std::stringstream input;
	input << "pm = { name = pm_hardwood percent = 0.33 }\n";
	input << "pm = { name = pm_sawmill }\n";

	mappers::ProductionMethodMapping rules;
	rules.loadRules(input);

	EXPECT_THAT(rules.getRules(), testing::UnorderedElementsAre(mappers::PMRule{"pm_hardwood", 0.33}, mappers::PMRule{"pm_sawmill"}));
}