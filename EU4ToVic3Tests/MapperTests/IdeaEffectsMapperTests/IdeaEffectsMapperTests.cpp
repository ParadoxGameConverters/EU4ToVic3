#include "IdeaEffectsMapper/IdeaEffectsMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_IdeaEffectsMapperTests, NonsenseIdeasReturnEmptyEffect)
{
	mappers::IdeaEffectsMapper mapper;
	std::stringstream input;
	mapper.loadMappingRules(input);

	std::set<std::string> ideas = {"idea", "another_idea"};

	const auto effect = mapper.getEffectForIdeas(ideas);

	EXPECT_EQ(0, effect.literacy);
	EXPECT_EQ(0, effect.adm);
	EXPECT_EQ(0, effect.dip);
	EXPECT_EQ(0, effect.mil);
	EXPECT_TRUE(effect.boostedInterestGroups.empty());
	EXPECT_TRUE(effect.suppressedInterestGroups.empty());
}
/*
 * Disabled as of 1.8+ until bolstering/suppression exists once again.
TEST(Mappers_IdeaEffectsMapperTests, effectCanBeCalculated)
{
	mappers::IdeaEffectsMapper mapper;
	std::stringstream input;
	input << "idea = { literacy = 1 adm = 2 dip = 3 mil = 4 ig = iggroup1 ig = iggroup2 noig = noiggroup1 noig = noiggroup2 }\n";
	input << "another_idea = { literacy = 3 adm = 4 dip = -5 mil = -6 ig = iggroup3 noig = iggroup1 noig = noiggroup2 }\n"; // cancelling out iggroup1
	mapper.loadMappingRules(input);

	std::set<std::string> ideas = {"idea", "another_idea"};

	const auto effect = mapper.getEffectForIdeas(ideas);

	EXPECT_EQ(4, effect.literacy);
	EXPECT_EQ(6, effect.adm);
	EXPECT_EQ(-2, effect.dip);
	EXPECT_EQ(-2, effect.mil);
	EXPECT_THAT(effect.boostedInterestGroups, testing::UnorderedElementsAre("iggroup2", "iggroup3"));
	EXPECT_THAT(effect.suppressedInterestGroups, testing::UnorderedElementsAre("noiggroup1", "noiggroup2"));
}
*/
TEST(Mappers_IdeaEffectsMapperTests, doubleBoostedInterestGroupBecomesRuling)
{
	mappers::IdeaEffectsMapper mapper;
	std::stringstream input;
	input << "idea = { literacy = 1 adm = 2 dip = 3 mil = 4 ig = iggroup1 ig = iggroup2 }\n";				  // iggroup1 gets boosted twice
	input << "another_idea = { literacy = 3 adm = 4 dip = -5 mil = -6 ig = iggroup1 noig = iggroup2}\n"; // cancelling out iggroup2
	mapper.loadMappingRules(input);

	std::set<std::string> ideas = {"idea", "another_idea"};

	const auto effect = mapper.getEffectForIdeas(ideas);

	EXPECT_THAT(effect.rulingInterestGroups, testing::UnorderedElementsAre("iggroup1"));
}
