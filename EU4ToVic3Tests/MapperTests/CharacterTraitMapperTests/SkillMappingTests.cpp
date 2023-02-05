#include "CharacterTraitMapper/SkillMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_SkillMappingTests, RulerTraitCanBeMatchedByPrimary)
{

	std::stringstream input;
	input << "sum = 10 prim = adm trait = theTrait";
	const mappers::SkillMapping mapping(input);

	const auto& match = mapping.matchTrait({5, 3, 3}); // sum 11, passes adm primary.

	ASSERT_TRUE(match);
	EXPECT_EQ("theTrait", *match);
}

TEST(Mappers_SkillMappingTests, RulerTraitWillFailForWrongPrimary)
{

	std::stringstream input;
	input << "sum = 10 prim = adm trait = theTrait";
	const mappers::SkillMapping mapping(input);

	const auto& match = mapping.matchTrait({3, 3, 5}); // sum 11, fails adm primary.

	EXPECT_FALSE(match);
}

TEST(Mappers_SkillMappingTests, RulerTraitWillFailForSubCount)
{

	std::stringstream input;
	input << "sum = 10 prim = adm trait = theTrait";
	const mappers::SkillMapping mapping(input);

	const auto& match = mapping.matchTrait({2, 1, 1}); // sum 4, fails sum.

	EXPECT_FALSE(match);
}

TEST(Mappers_SkillMappingTests, RulerTraitCanBeMatchedGenerallyBySum)
{

	std::stringstream input;
	input << "sum = 10 trait = theTrait";
	const mappers::SkillMapping mapping(input);

	const auto& match = mapping.matchTrait({5, 3, 3}); // sum 11, passes sum.

	ASSERT_TRUE(match);
	EXPECT_EQ("theTrait", *match);
}

TEST(Mappers_SkillMappingTests, RulerTraitWillFailForGeneralSubCount)
{

	std::stringstream input;
	input << "sum = 10 trait = theTrait";
	const mappers::SkillMapping mapping(input);

	const auto& match = mapping.matchTrait({1, 1, 2}); // sum 4, fails sum.

	EXPECT_FALSE(match);
}
