#include "CultureMapper/CultureTraitMapper/CultureTraitMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_CultureTraitMapperTests, noMatchesGiveEmptyOptional)
{
	mappers::CultureTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/culture_trait_heritage_map.txt",
		 "TestFiles/configurables/culture_trait_language_map.txt",
		 "TestFiles/configurables/culture_trait_tradition_map.txt");

	EXPECT_FALSE(mapper.getTraitsForCulture("nonsense", "nonsense"));
}

TEST(Mappers_CultureTraitMapperTests, heritageMatchOverridesGroupMatch)
{
	mappers::CultureTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/culture_trait_heritage_map.txt",
		 "TestFiles/configurables/culture_trait_language_map.txt",
		 "TestFiles/configurables/culture_trait_tradition_map.txt");

	const auto match = mapper.getTraitsForCulture("culture1", "additional_group");
	ASSERT_TRUE(match);

	EXPECT_EQ("testtrait", match->getHeritage());
	EXPECT_EQ("testable", match->getEthnicity());
}

TEST(Mappers_CultureTraitMapperTests, heritageGroupMatchMatches)
{
	mappers::CultureTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/culture_trait_heritage_map.txt",
		 "TestFiles/configurables/culture_trait_language_map.txt",
		 "TestFiles/configurables/culture_trait_tradition_map.txt");

	const auto match = mapper.getTraitsForCulture("undefined", "additional_group");
	ASSERT_TRUE(match);

	EXPECT_EQ("testtrait3", match->getHeritage());
	EXPECT_EQ("testable3", match->getEthnicity());
}

TEST(Mappers_CultureTraitMapperTests, languageMatchOverridesGroupMatch)
{
	mappers::CultureTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/culture_trait_heritage_map.txt",
		 "TestFiles/configurables/culture_trait_language_map.txt",
		 "TestFiles/configurables/culture_trait_tradition_map.txt");

	const auto match = mapper.getTraitsForCulture("culture1", "additional_group");
	ASSERT_TRUE(match);

	EXPECT_EQ("test_lang", match->getLanguage());
}

TEST(Mappers_CultureTraitMapperTests, languageGroupMatchMatches)
{
	mappers::CultureTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/culture_trait_heritage_map.txt",
		 "TestFiles/configurables/culture_trait_language_map.txt",
		 "TestFiles/configurables/culture_trait_tradition_map.txt");

	const auto match = mapper.getTraitsForCulture("undefined", "additional_group");
	ASSERT_TRUE(match);

	EXPECT_EQ("test_lang3", match->getLanguage());
}

TEST(Mappers_CultureTraitMapperTests, traditionMatchOverridesGroupMatch)
{
	mappers::CultureTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/culture_trait_heritage_map.txt",
		 "TestFiles/configurables/culture_trait_language_map.txt",
		 "TestFiles/configurables/culture_trait_tradition_map.txt");

	const auto match = mapper.getTraitsForCulture("culture1", "additional_group");
	ASSERT_TRUE(match);

	EXPECT_THAT(match->getTraditions(), testing::UnorderedElementsAre("test_trad5", "test_trad6"));
}

TEST(Mappers_CultureTraitMapperTests, traditionGroupMatchMatches)
{
	mappers::CultureTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/culture_trait_heritage_map.txt",
		 "TestFiles/configurables/culture_trait_language_map.txt",
		 "TestFiles/configurables/culture_trait_tradition_map.txt");

	const auto match = mapper.getTraitsForCulture("undefined", "additional_group");
	ASSERT_TRUE(match);

	EXPECT_THAT(match->getTraditions(), testing::UnorderedElementsAre("test_trad3", "test_trad4"));
}
