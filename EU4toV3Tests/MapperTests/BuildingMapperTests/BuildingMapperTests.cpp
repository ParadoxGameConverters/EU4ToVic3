#include "BuildingMapper/BuildingMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_BuildingMapperTests, rulesCanBeLoaded)
{
	mappers::BuildingMapper mapper;
	mapper.loadBuildingMappings("TestFiles/configurables/buildings_map.txt");

	EXPECT_THAT(mapper.getVic3Buildings("wharf"), testing::UnorderedElementsAre("building_fishing_wharf"));
	EXPECT_THAT(mapper.getVic3Buildings("mills"), testing::UnorderedElementsAre("building_glassworks", "building_paper_mills", "building_logging_camp"));
}