#include "LeaderTraits/LeaderTrait.h"
#include "gtest/gtest.h"

TEST(Mappers_LeaderTraitTests, primitivesDefaultToZero)
{
	std::stringstream input;
	const mappers::LeaderTrait trait(input, "trait");

	ASSERT_EQ("trait", trait.getName());
	ASSERT_EQ(0, trait.getFire());
	ASSERT_EQ(0, trait.getManeuver());
	ASSERT_EQ(0, trait.getOther());
	ASSERT_EQ(0, trait.getShock());
	ASSERT_EQ(0, trait.getSiege());
}

TEST(Mappers_LeaderTraitTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "fire = 1 maneuver = 2 other = 3 shock = 4 siege = 5";
	const mappers::LeaderTrait trait(input, "trait");

	ASSERT_EQ("trait", trait.getName());
	ASSERT_EQ(1, trait.getFire());
	ASSERT_EQ(2, trait.getManeuver());
	ASSERT_EQ(3, trait.getOther());
	ASSERT_EQ(4, trait.getShock());
	ASSERT_EQ(5, trait.getSiege());
}

TEST(Mappers_LeaderTraitTests, straightPrimitiveMatchesWithPrimitiveEqualOrHigher)
{
	std::stringstream input1;
	input1 << "fire = 4";
	const mappers::LeaderTrait trait1(input1, "trait");
	std::stringstream input2;
	input2 << "maneuver = 4";
	const mappers::LeaderTrait trait2(input2, "trait");
	std::stringstream input3;
	input3 << "shock = 4";
	const mappers::LeaderTrait trait3(input3, "trait");
	std::stringstream input4;
	input4 << "siege = 5";
	const mappers::LeaderTrait trait4(input4, "trait");

	ASSERT_TRUE(trait1.matches(4, 0, 0, 0));
	ASSERT_TRUE(trait2.matches(0, 0, 4, 0));
	ASSERT_TRUE(trait3.matches(0, 9, 0, 0));
	ASSERT_TRUE(trait4.matches(0, 0, 0, 6));
}

TEST(Mappers_LeaderTraitTests, straightPrimitiveMatchFailsWithPrimitiveLower)
{
	std::stringstream input1;
	input1 << "fire = 4";
	const mappers::LeaderTrait trait1(input1, "trait");
	std::stringstream input2;
	input2 << "maneuver = 4";
	const mappers::LeaderTrait trait2(input2, "trait");
	std::stringstream input3;
	input3 << "shock = 4";
	const mappers::LeaderTrait trait3(input3, "trait");
	std::stringstream input4;
	input4 << "siege = 5";
	const mappers::LeaderTrait trait4(input4, "trait");

	ASSERT_FALSE(trait1.matches(3, 0, 0, 0));
	ASSERT_FALSE(trait2.matches(0, 0, 2, 0));
	ASSERT_FALSE(trait3.matches(0, 1, 0, 0));
	ASSERT_FALSE(trait4.matches(0, 0, 0, 0));
}

TEST(Mappers_LeaderTraitTests, otherMatchesWhenOtherPrimitivesSumEqualOrHigher)
{
	std::stringstream input1;
	input1 << "fire = 4 other = 10";
	const mappers::LeaderTrait trait1(input1, "trait");

	ASSERT_TRUE(trait1.matches(4, 5, 5, 5)); // other sum = 15
}

TEST(Mappers_LeaderTraitTests, otherMatchFailsWhenOtherPrimitivesSumLower)
{
	std::stringstream input1;
	input1 << "fire = 4 other = 10";
	const mappers::LeaderTrait trait1(input1, "trait");

	ASSERT_FALSE(trait1.matches(4, 3, 3, 3)); // other sum = 9
}
