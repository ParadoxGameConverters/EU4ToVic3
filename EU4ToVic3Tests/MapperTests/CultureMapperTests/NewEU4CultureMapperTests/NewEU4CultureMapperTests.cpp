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

	const auto addMatch = mapper.getAddTraitsForCulture("culture3");
	const auto remMatch = mapper.getRemoveTraitsForCulture("culture3");
	ASSERT_FALSE(addMatch.empty());
	ASSERT_FALSE(remMatch.empty());

	EXPECT_THAT(addMatch, testing::UnorderedElementsAre("testaddedtrait"));
	EXPECT_THAT(remMatch, testing::UnorderedElementsAre("testtrait2"));
}
