#include "BuildingMapper/BuildingMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_BuildingMapperTests, RulesCanBeLoaded)
{
	mappers::BuildingMapper mapper;

	EXPECT_TRUE(mapper.getVic3Buildings("wharf").empty());

	mapper.loadBuildingMappings("TestFiles/configurables/economy/buildings_map.txt");

	EXPECT_THAT(mapper.getVic3Buildings("wharf"), testing::UnorderedElementsAre("building_fishing_wharf"));
	EXPECT_THAT(mapper.getVic3Buildings("mills"), testing::UnorderedElementsAre("building_glassworks", "building_paper_mill", "building_logging_camp"));
}