#include "CultureMapper/CultureTraitMapper/CultureTraitMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_CultureTraitMapperTests, noMatchesGiveEmptyOptional)
{
	mappers::CultureTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/culture_trait_map.txt");

	EXPECT_FALSE(mapper.getTraitsForCulture("nonsense", "nonsense"));
}

TEST(Mappers_CultureTraitMapperTests, cultureMatchMatches)
{
	mappers::CultureTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/culture_trait_map.txt");

	const auto match = mapper.getTraitsForCulture("culture1", "additional_group");
	ASSERT_TRUE(match);

	EXPECT_THAT(match->getTraits(), testing::UnorderedElementsAre("testtrait", "testtrait2"));
	EXPECT_EQ("testable", match->getEthnicity());
}

TEST(Mappers_CultureTraitMapperTests, cultureGroupMatchMatches)
{
	mappers::CultureTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/culture_trait_map.txt");

	const auto match = mapper.getTraitsForCulture("undefined", "additional_group");
	ASSERT_TRUE(match);

	EXPECT_THAT(match->getTraits(), testing::UnorderedElementsAre("testtrait3"));
	EXPECT_EQ("testable3", match->getEthnicity());
}
