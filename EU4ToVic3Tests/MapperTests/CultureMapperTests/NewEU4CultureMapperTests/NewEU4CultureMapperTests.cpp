#include "CultureMapper/NewEU4CultureMapper/NewEU4CultureMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_NewEU4CultureMapperTests, noMatchesGiveEmptySet)
{
	mappers::NewEU4CultureMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/new_eu4_culture_map.txt");

	EXPECT_TRUE(mapper.getAddTraitsForCulture("nonsense").empty());
}

TEST(Mappers_NewEU4CultureMapperTests, cultureMatchMatches)
{
	mappers::NewEU4CultureMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/new_eu4_culture_map.txt");

	EXPECT_THAT(mapper.getAddTraitsForCulture("culture3"), testing::UnorderedElementsAre("testaddedtrait"));
	EXPECT_THAT(mapper.getRemoveTraitsForCulture("culture3"), testing::UnorderedElementsAre("testtrait2"));
}
