#include "ReligionMapper/ReligionGroupMapper/ReligionGroupMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ReligionGroupMapperTests, religionGroupMappingsCanBeRetrieved)
{
	mappers::ReligionGroupMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/religion_group_map.txt");

	const auto& match = mapper.getMappingForEU4ReligionGroup("mod_group_1");
	ASSERT_TRUE(match);

	EXPECT_EQ("religiontrait_2", match->getTrait());
	EXPECT_THAT(match->getEU4Groups(), testing::UnorderedElementsAre("vanilla_group_2", "mod_group_1"));
	EXPECT_THAT(match->getTaboos(), testing::UnorderedElementsAre("liquor", "wine"));
	EXPECT_EQ("religion_2", match->getIcon());
}

TEST(Mappers_ReligionGroupMapperTests, MisMatchReturnsNullopt)
{
	mappers::ReligionGroupMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/religion_group_map.txt");

	EXPECT_FALSE(mapper.getMappingForEU4ReligionGroup("unmapped"));
}
