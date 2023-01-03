#include "IdeaEffectsMapper/IdeaEffectsMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_IdeaEffectsMappingTests, DefaultsDefaultToDefaults)
{

	std::stringstream input;
	const mappers::IdeaEffectsMapping mapping(input);

	EXPECT_EQ(0, mapping.getLiteracy());
	EXPECT_EQ(0, mapping.getAdm());
	EXPECT_EQ(0, mapping.getDip());
	EXPECT_EQ(0, mapping.getMil());
	EXPECT_TRUE(mapping.getBoostedInterestGroups().empty());
	EXPECT_TRUE(mapping.getSuppressedInterestGroups().empty());
}

TEST(Mappers_IdeaEffectsMappingTests, RulesCanBeLoaded)
{
	std::stringstream input;
	input << "literacy = 1 adm = 2 dip = 3 mil = 4 ig = iggroup1 ig = iggroup2 noig = noiggroup1 noig = noiggroup2";
	const mappers::IdeaEffectsMapping mapping(input);

	EXPECT_EQ(1, mapping.getLiteracy());
	EXPECT_EQ(2, mapping.getAdm());
	EXPECT_EQ(3, mapping.getDip());
	EXPECT_EQ(4, mapping.getMil());
	EXPECT_THAT(mapping.getBoostedInterestGroups(), testing::UnorderedElementsAre("iggroup1", "iggroup2"));
	EXPECT_THAT(mapping.getSuppressedInterestGroups(), testing::UnorderedElementsAre("noiggroup1", "noiggroup2"));
}
