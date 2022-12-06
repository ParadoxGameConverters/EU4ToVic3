#include "ReligionMapper/ReligionGroupMapper/ReligionGroupMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ReligionGroupMapperTests, religionGroupMappingsCanBeRetrieved)
{
	mappers::ReligionGroupMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/religion_group_map.txt");

	const auto& match = mapper.getMappingForEU4ReligionGroup("gnostic");
	ASSERT_TRUE(match);

	EXPECT_EQ("christian", match->getTrait());
	EXPECT_THAT(match->getEU4Groups(), testing::UnorderedElementsAre("christian", "gnostic"));
	EXPECT_TRUE(match->getTaboos().empty());
	EXPECT_EQ("protestant", match->getIcon());
}

TEST(Mappers_ReligionGroupMapperTests, MisMatchReturnsNullopt)
{
	mappers::ReligionGroupMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/religion_group_map.txt");

	EXPECT_FALSE(mapper.getMappingForEU4ReligionGroup("unmapped"));
}
