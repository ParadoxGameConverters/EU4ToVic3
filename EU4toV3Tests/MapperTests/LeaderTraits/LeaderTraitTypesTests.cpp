#include "LeaderTraits/LeaderTraitTypes.h"
#include "gtest/gtest.h"

TEST(Mappers_LeaderTraitTypesTests, traitsDefaultToEmpty)
{
	std::stringstream input;
	const mappers::LeaderTraitTypes traits(input);

	ASSERT_TRUE(traits.getTraits().empty());
}

TEST(Mappers_LeaderTraitTypesTests, traitsCanBeLoaded)
{
	std::stringstream input;
	input << "trait1 = { fire = 1 maneuver = 2 other = 3 shock = 4 siege = 5 }";
	input << "trait2 = { fire = 1 maneuver = 2 other = 3 shock = 4 siege = 5 }";
	input << "trait3 = { fire = 1 maneuver = 2 other = 3 shock = 4 siege = 5 }";
	const mappers::LeaderTraitTypes traits(input);

	ASSERT_EQ(3, traits.getTraits().size());
	ASSERT_EQ("trait1", traits.getTraits()[0].getName());
	ASSERT_EQ("trait2", traits.getTraits()[1].getName());
	ASSERT_EQ("trait3", traits.getTraits()[2].getName());
}
