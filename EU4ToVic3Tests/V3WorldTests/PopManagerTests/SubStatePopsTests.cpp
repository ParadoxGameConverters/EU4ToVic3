#include "PopManager/Pops/SubStatePops.h"
#include "gtest/gtest.h"

TEST(V3World_SubStatePopsTests, selectsDominantCultureAndReligion)
{
	V3::SubStatePops subStatePops;
	subStatePops.addPop(V3::Pop("culture_a", "religion_a", "laborers", 100));
	subStatePops.addPop(V3::Pop("culture_b", "religion_b", "farmers", 250));
	subStatePops.addPop(V3::Pop("culture_b", "religion_a", "clergy", 75));
	subStatePops.addPop(V3::Pop("", "", "aristocrats", 999));

	ASSERT_TRUE(subStatePops.getDominantCulture());
	ASSERT_TRUE(subStatePops.getDominantReligion());
	EXPECT_EQ("culture_b", *subStatePops.getDominantCulture());
	EXPECT_EQ("religion_b", *subStatePops.getDominantReligion());
}