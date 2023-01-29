#include "BuildingMapper/ProductionMethodMapper/ProductionMethodMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ProductionMethodMapperTests, RulesCanBeLoaded)
{
	mappers::ProductionMethodMapper mapper;

	EXPECT_TRUE(mapper.getRules().empty());

	mapper.loadRules("TestFiles/configurables/economy/production_method_rules.txt");

	EXPECT_THAT(mapper.getRules().at("building_government_administration"), testing::UnorderedElementsAre(mappers::PMRule{"pm_vertical_filing_cabinets"}));
	EXPECT_THAT(mapper.getRules().at("building_logging_camp"),
		 testing::UnorderedElementsAre(mappers::PMRule{"pm_saw_mills"}, mappers::PMRule{"pm_hardwood", 0.33}));
}