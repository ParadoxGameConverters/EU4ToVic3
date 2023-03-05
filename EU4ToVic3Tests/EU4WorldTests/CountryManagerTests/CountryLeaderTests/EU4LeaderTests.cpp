#include "CountryManager/CountryLeader/EU4Leader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(EU4World_EU4LeaderTests, primitivesDefaultToDefaultValues)
{
	const EU4::Leader leaderParser;
	const auto& leader = leaderParser.getCharacter();

	EXPECT_TRUE(leader.leaderName.empty());
	EXPECT_FALSE(leader.female);
	EXPECT_FALSE(leader.activationDate.isSet());
	EXPECT_EQ(0, leader.fire);
	EXPECT_EQ(0, leader.shock);
	EXPECT_EQ(0, leader.maneuver);
	EXPECT_EQ(0, leader.shock);
	EXPECT_EQ(0, leader.leaderID);
}

TEST(EU4World_EU4LeaderTests, primitiveLeaderCanBeLoaded)
{
	std::stringstream input;
	input << "name=Boby\n";
	input << "type=admiral\n";
	input << "female=true\n";
	input << "manuever=3\n";
	input << "fire=1\n";
	input << "shock=2\n";
	input << "siege=9\n";
	input << "activation=6.6.6\n";
	input << "personality=aggressive\n";
	input << "id={\n";
	input << "\tid=32\n";
	input << "}\n";
	EU4::Leader leaderParser;
	leaderParser.parseLeader(input);
	const auto& leader = leaderParser.getCharacter();

	EXPECT_EQ("Boby", leader.leaderName);
	EXPECT_TRUE(leader.female);
	EXPECT_EQ(date("6.6.6"), leader.activationDate);
	EXPECT_EQ(1, leader.fire);
	EXPECT_EQ(2, leader.shock);
	EXPECT_EQ(3, leader.maneuver);
	EXPECT_EQ(9, leader.siege);
	EXPECT_EQ(32, leader.leaderID);
	EXPECT_EQ("aggressive", leader.leaderTrait);
}

TEST(EU4World_EU4LeaderTests, simpleCharacterCanBeLoaded)
{
	std::stringstream input;
	input << "id={\n";
	input << "	id=3052\n";
	input << "	type=48\n";
	input << "}\n";
	input << "name=\"Marie\"\n";
	input << "country=\"FRA\"\n";
	input << "DIP=4\n";
	input << "ADM=4\n";
	input << "MIL=4\n";
	input << "female=yes\n";
	input << "culture=occitain\n";
	input << "religion=catholic\n";
	input << "personalities={\n";
	input << "	zealot_personality=yes\n";
	input << "}\n";
	input << "dynasty=\"d'Anjou\"\n";
	input << "birth_date=1404.10.14\n";
	input << "country_of_origin=PRO\n";

	EU4::Leader leaderParser;
	leaderParser.parseRuler(input);
	const auto& character = leaderParser.getCharacter();

	EXPECT_EQ(3052, character.monarchID);
	EXPECT_EQ("Marie", character.name);
	EXPECT_EQ("FRA", character.country);
	EXPECT_EQ(4, character.adm);
	EXPECT_EQ(4, character.mil);
	EXPECT_EQ(4, character.dip);
	EXPECT_EQ(true, character.female);
	EXPECT_EQ("occitain", character.culture);
	EXPECT_EQ("catholic", character.religion);
	EXPECT_THAT(character.traits, testing::UnorderedElementsAre("zealot_personality"));
	EXPECT_EQ("d'Anjou", character.dynasty);
	EXPECT_EQ(date("1404.10.14"), character.birthDate);
	EXPECT_EQ("PRO", character.countryOfOrigin);
}

TEST(EU4World_EU4LeaderTests, complexCharacterCanBeLoaded)
{
	std::stringstream input;
	input << "id={\n";
	input << "	id=3052\n";
	input << "	type=48\n";
	input << "}\n";
	input << "name=\"Marie\"\n";
	input << "country=\"FRA\"\n";
	input << "DIP=4\n";
	input << "ADM=4\n";
	input << "MIL=4\n";
	input << "female=yes\n";
	input << "culture=occitain\n";
	input << "religion=catholic\n";
	input << "personalities={\n";
	input << "	zealot_personality=yes\n";
	input << "}\n";
	input << "leader = {\n";
	input << "	name=Boby\n";
	input << "	type=admiral\n";
	input << "	female=true\n";
	input << "	manuever=3\n";
	input << "	fire=1\n";
	input << "	shock=2\n";
	input << "	siege=9\n";
	input << "	activation=6.6.6\n";
	input << "	personality=aggressive\n";
	input << "	id={\n";
	input << "		id=32\n";
	input << "	}\n";
	input << "}\n";
	input << "dynasty=\"d'Anjou\"\n";
	input << "birth_date=1404.10.14\n";
	input << "country_of_origin=PRO\n";

	EU4::Leader leaderParser;
	leaderParser.parseRuler(input);
	const auto& character = leaderParser.getCharacter();

	EXPECT_EQ(32, character.leaderID);
	EXPECT_EQ(3052, character.monarchID);
	EXPECT_EQ("Marie", character.name);
	EXPECT_EQ("FRA", character.country);
	EXPECT_EQ(4, character.adm);
	EXPECT_EQ(4, character.mil);
	EXPECT_EQ(4, character.dip);
	EXPECT_EQ(true, character.female);
	EXPECT_EQ("occitain", character.culture);
	EXPECT_EQ("catholic", character.religion);
	EXPECT_THAT(character.traits, testing::UnorderedElementsAre("zealot_personality"));
	EXPECT_EQ("d'Anjou", character.dynasty);
	EXPECT_EQ(date("1404.10.14"), character.birthDate);
	EXPECT_EQ("PRO", character.countryOfOrigin);
	EXPECT_EQ("Boby", character.leaderName);
	EXPECT_EQ(date("6.6.6"), character.activationDate);
	EXPECT_EQ(1, character.fire);
	EXPECT_EQ(2, character.shock);
	EXPECT_EQ(3, character.maneuver);
	EXPECT_EQ(9, character.siege);
	EXPECT_EQ("aggressive", character.leaderTrait);
}
