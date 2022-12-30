#include "CharacterTraitMapper/CharacterTraitMapper.h"
#include "CountryManager/CountryLeader/EU4Character.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_CharacterTraitMapperTests, personalitiesCanBeMatched)
{
	mappers::CharacterTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/character_traits.txt");

	EXPECT_EQ("ambitious", *mapper.getPersonality("glory_seeker_personality"));
}

TEST(Mappers_CharacterTraitMapperTests, personalitiesWillFailForNonsense)
{
	mappers::CharacterTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/character_traits.txt");

	EXPECT_FALSE(mapper.getPersonality("nonsense"));
}

TEST(Mappers_CharacterTraitMapperTests, skillTraitsGrabsGlobalsForExplorersAndConquistadors)
{
	mappers::CharacterTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/character_traits.txt");

	EU4::Character character1;
	character1.leaderType = "explorer";
	EU4::Character character2;
	character2.leaderType = "conquistador";

	EXPECT_THAT(mapper.getSkillTraits(character1), testing::UnorderedElementsAre("explorer"));
	EXPECT_THAT(mapper.getSkillTraits(character2), testing::UnorderedElementsAre("surveyor"));
}

TEST(Mappers_CharacterTraitMapperTests, skillTraitsMatchesActualSkillsForGeneralsAndAdmirals)
{
	mappers::CharacterTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/character_traits.txt");

	EU4::Character character1;
	character1.leaderType = "general";
	character1.fire = 6;
	character1.shock = 5;
	character1.maneuver = 5;
	character1.siege = 5;
	EU4::Character character2;
	character2.fire = 5;
	character2.shock = 5;
	character2.maneuver = 6;
	character2.siege = 5;
	character2.leaderType = "admiral";

	EXPECT_THAT(mapper.getSkillTraits(character1), testing::UnorderedElementsAre("expert_defensive_strategist"));
	EXPECT_THAT(mapper.getSkillTraits(character2), testing::UnorderedElementsAre("expert_convoy_raider"));
}

TEST(Mappers_CharacterTraitMapperTests, skillTraitsReturnsEmptyForJunkLeadersWithNoMatches)
{
	mappers::CharacterTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/character_traits.txt");

	EU4::Character character1;
	character1.leaderType = "general";
	character1.fire = 1;
	character1.shock = 0;
	character1.maneuver = 0;
	character1.siege = 0;
	EU4::Character character2;
	character2.fire = 0;
	character2.shock = 0;
	character2.maneuver = 0;
	character2.siege = 1;
	character2.leaderType = "admiral";

	EXPECT_TRUE(mapper.getSkillTraits(character1).empty());
	EXPECT_TRUE(mapper.getSkillTraits(character2).empty());
}

TEST(Mappers_CharacterTraitMapperTests, skillTraitsMatchesActualSkillsForRulers)
{
	mappers::CharacterTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/character_traits.txt");

	EU4::Character character1;
	character1.ruler = true;
	character1.adm = 6;
	character1.dip = 5;
	character1.mil = 5;

	EXPECT_THAT(mapper.getSkillTraits(character1), testing::UnorderedElementsAre("expert_colonial_administrator"));
}

TEST(Mappers_CharacterTraitMapperTests, skillTraitsReturnsEmptyForJunkRulersWithNoMatches)
{
	mappers::CharacterTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/character_traits.txt");

	EU4::Character character1;
	character1.ruler = true;
	character1.adm = 1;
	character1.dip = 0;
	character1.mil = 0;

	EXPECT_TRUE(mapper.getSkillTraits(character1).empty());
}

TEST(Mappers_CharacterTraitMapperTests, skillTraitsCombinesRulerAndLeaderTraits)
{
	mappers::CharacterTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/character_traits.txt");

	EU4::Character character1;
	character1.leaderType = "general";
	character1.fire = 6;
	character1.shock = 5;
	character1.maneuver = 5;
	character1.siege = 5;
	character1.ruler = true;
	character1.adm = 6;
	character1.dip = 5;
	character1.mil = 5;

	EXPECT_THAT(mapper.getSkillTraits(character1), testing::UnorderedElementsAre("expert_colonial_administrator", "expert_defensive_strategist"));
}

TEST(Mappers_CharacterTraitMapperTests, gratisIncompetenciesCanBeReturned)
{
	mappers::CharacterTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/character_traits.txt");

	EXPECT_EQ("alcoholic", mapper.getGratisIncompetency(-76));
	EXPECT_EQ("alcoholic", mapper.getGratisIncompetency(0));
	EXPECT_EQ("opium_addiction", mapper.getGratisIncompetency(3));
	EXPECT_EQ("alcoholic", mapper.getGratisIncompetency(4988));
}

TEST(Mappers_CharacterTraitMapperTests, gratisVeterancyCanBeReturned)
{
	mappers::CharacterTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/character_traits.txt");

	EXPECT_EQ("beetle_ear", mapper.getGratisVeterancy(-76));
	EXPECT_EQ("beetle_ear", mapper.getGratisVeterancy(0));
	EXPECT_EQ("beetle_ear", mapper.getGratisVeterancy(3));
	EXPECT_EQ("wounded", mapper.getGratisVeterancy(4988));
}

TEST(Mappers_CharacterTraitMapperTests, gratisAgeismCanBeReturned)
{
	mappers::CharacterTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/character_traits.txt");

	EXPECT_EQ("cancer", mapper.getGratisAgeism(-76));
	EXPECT_EQ("cancer", mapper.getGratisAgeism(0));
	EXPECT_EQ("cancer", mapper.getGratisAgeism(3));
	EXPECT_EQ("senile", mapper.getGratisAgeism(4988));
}

TEST(Mappers_CharacterTraitMapperTests, gratisDisordersCanBeReturned)
{
	mappers::CharacterTraitMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/character_traits.txt");

	EXPECT_EQ("expensive_tastes", mapper.getGratisDisorder(-76));
	EXPECT_EQ("expensive_tastes", mapper.getGratisDisorder(0));
	EXPECT_EQ("syphilis", mapper.getGratisDisorder(3));
	EXPECT_EQ("expensive_tastes", mapper.getGratisDisorder(4988));
}
