#include "PopManager/Pops/StatePops.h"
#include "PopManager/Pops/SubStatePops.h"
#include "gtest/gtest.h"

TEST(V3World_StatePopsTests, aggregatesDominantCultureAndReligionAcrossSubstates)
{
	V3::SubStatePops firstSubState;
	firstSubState.addPop(V3::Pop("culture_a", "religion_a", "laborers", 150));
	firstSubState.addPop(V3::Pop("culture_b", "religion_b", "farmers", 100));

	V3::SubStatePops secondSubState;
	secondSubState.addPop(V3::Pop("culture_b", "religion_b", "laborers", 200));
	secondSubState.addPop(V3::Pop("", "", "clergy", 1000));

	V3::StatePops statePops;
	statePops.addSubStatePops(firstSubState);
	statePops.addSubStatePops(secondSubState);

	ASSERT_TRUE(statePops.getDominantCulture());
	ASSERT_TRUE(statePops.getDominantReligion());
	EXPECT_EQ("culture_b", *statePops.getDominantCulture());
	EXPECT_EQ("religion_b", *statePops.getDominantReligion());
}