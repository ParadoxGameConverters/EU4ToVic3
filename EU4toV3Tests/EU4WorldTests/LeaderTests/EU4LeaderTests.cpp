#include "Leader/EU4Leader.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4LeaderTests, primitivesDefaultToDefaultValues)
{
	std::stringstream input;
	const EU4::Leader leader(input);

	ASSERT_TRUE(leader.getName().empty());
	ASSERT_FALSE(leader.isFemale());
	ASSERT_EQ(date("1.1.1"), leader.getActivationDate());
	ASSERT_EQ(0, leader.getFire());
	ASSERT_EQ(0, leader.getShock());
	ASSERT_EQ(0, leader.getManeuver());
	ASSERT_EQ(0, leader.getSiege());
	ASSERT_EQ(0, leader.getID());
}

TEST(EU4World_EU4LeaderTests, primitivesCanBeLoaded)
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
	input << "id={\n";
	input << "\tid=32\n";
	input << "}\n";
	const EU4::Leader leader(input);

	ASSERT_EQ("Boby", leader.getName());
	ASSERT_TRUE(leader.isFemale());
	ASSERT_EQ(date("6.6.6"), leader.getActivationDate());
	ASSERT_EQ(1, leader.getFire());
	ASSERT_EQ(2, leader.getShock());
	ASSERT_EQ(3, leader.getManeuver());
	ASSERT_EQ(9, leader.getSiege());
	ASSERT_EQ(32, leader.getID());
}

TEST(EU4World_EU4LeaderTests, typeDeterminesUniform)
{
	std::stringstream input;
	input << "type=admiral\n";
	const EU4::Leader leader(input);

	std::stringstream input2;
	input2 << "type=explorer\n";
	const EU4::Leader leader2(input2);

	std::stringstream input3;
	input3 << "type=general\n";
	const EU4::Leader leader3(input3);

	std::stringstream input4;
	input4 << "type=conquistador\n";
	const EU4::Leader leader4(input4);
	
	ASSERT_FALSE(leader.isLand());
	ASSERT_FALSE(leader2.isLand());
	ASSERT_TRUE(leader3.isLand());
	ASSERT_TRUE(leader4.isLand());
}

TEST(EU4World_EU4LeaderTests, wrongTypeThrowsWarning)
{
	std::stringstream input;
	input << "type=troubadour\n";
	const EU4::Leader leader(input);

	std::stringstream log;
	auto* stdOutBuf = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	ASSERT_FALSE(leader.isLand());
	std::cout.rdbuf(stdOutBuf);

	ASSERT_EQ(" [WARNING] Unknown leader type troubadour\n", log.str());	
}
