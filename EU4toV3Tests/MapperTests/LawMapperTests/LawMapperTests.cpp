#include "LawMapper/LawMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_LawMapperTests, LawsGroupsDefaultToEmpty)
{
	const mappers::LawMapper mapper;

	EXPECT_TRUE(mapper.getLawGroups().empty());
}

TEST(Mappers_LawMapperTests, LawsGroupsCanBeLoaded)
{
	mappers::LawMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/law_map.txt");
	const auto& lawGroups = mapper.getLawGroups();

	ASSERT_EQ(3, lawGroups.size());
	EXPECT_TRUE(lawGroups.contains("lawgroup_1"));
	EXPECT_EQ(1, lawGroups.at("lawgroup_1").size());
	EXPECT_TRUE(lawGroups.contains("lawgroup_2"));
	EXPECT_EQ(2, lawGroups.at("lawgroup_2").size());
	EXPECT_TRUE(lawGroups.contains("lawgroup_3"));
	EXPECT_EQ(1, lawGroups.at("lawgroup_3").size());
}
