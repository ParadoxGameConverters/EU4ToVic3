#include "BuildingMapper/BuildingMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_BuildingMappingTests, EmptyInputCreatesEmptyDefault)
{

	std::stringstream input;
	input << "";
	const mappers::BuildingMapping buildingMapping(input);

	EXPECT_TRUE(buildingMapping.getVic3Buildings().empty());
	EXPECT_TRUE(buildingMapping.getEU4Building().empty());
}

TEST(Mappers_BuildingMappingTests, RulesCanBeLoaded)
{

	std::stringstream input;
	input << "eu4 = building_1 vic3 = { building_1 building_2 }\n";

	const mappers::BuildingMapping buildingMapping(input);

	EXPECT_THAT(buildingMapping.getVic3Buildings(), testing::UnorderedElementsAre("building_1", "building_2"));
	EXPECT_EQ("building_1", buildingMapping.getEU4Building());
}