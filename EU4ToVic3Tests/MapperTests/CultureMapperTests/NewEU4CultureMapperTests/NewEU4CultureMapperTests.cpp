#include "CultureMapper/NewEU4CultureMapper/NewEU4CultureMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_NewEU4CultureMapperTests, noMatchesGiveEmptySet)
{
	mappers::NewEU4CultureMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/new_eu4_culture_map.txt");

	EXPECT_TRUE(mapper.getAddTraditionsForCulture("nonsense").empty());
	EXPECT_TRUE(mapper.getRemoveTraditionsForCulture("nonsense").empty());
	EXPECT_FALSE(mapper.getReplaceHeritageForCulture("nonsense"));
	EXPECT_FALSE(mapper.getReplaceLanguageForCulture("nonsense"));
}

TEST(Mappers_NewEU4CultureMapperTests, cultureMatchMatches)
{
	mappers::NewEU4CultureMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/new_eu4_culture_map.txt");

	EXPECT_THAT(mapper.getAddTraditionsForCulture("culture3"), testing::UnorderedElementsAre("testaddedtrait"));
	EXPECT_THAT(mapper.getRemoveTraditionsForCulture("culture3"), testing::UnorderedElementsAre("testtrait2"));
	EXPECT_EQ("new_language", mapper.getReplaceLanguageForCulture("culture3"));
	EXPECT_EQ("new_heritage", mapper.getReplaceHeritageForCulture("culture3"));
}
