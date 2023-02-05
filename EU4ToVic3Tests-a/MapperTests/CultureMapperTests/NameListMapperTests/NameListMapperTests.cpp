#include "CultureMapper/NameListMapper/NameListMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_NameListMapperTests, noMatchesGiveEmptyOptional)
{
	mappers::NameListMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/name_list_map.txt");

	EXPECT_FALSE(mapper.getNamesForCulture("nonsense", "nonsense"));
}

TEST(Mappers_NameListMapperTests, cultureMatchMatches)
{
	mappers::NameListMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/name_list_map.txt");

	const auto match = mapper.getNamesForCulture("culture1", "additional_group");
	ASSERT_TRUE(match);

	EXPECT_EQ("name_list_test1", match->getNamePool());
}

TEST(Mappers_NameListMapperTests, cultureGroupMatchMatches)
{
	mappers::NameListMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/name_list_map.txt");

	const auto match = mapper.getNamesForCulture("undefined", "additional_group");
	ASSERT_TRUE(match);

	EXPECT_EQ("name_list_test2", match->getNamePool());
}
