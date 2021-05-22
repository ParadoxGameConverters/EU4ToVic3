#include "Relations/EU4RelationDetails.h"
#include "gtest/gtest.h"

TEST(EU4World_Relations_RelationDetailsTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const EU4::EU4RelationDetails details(input);

	ASSERT_EQ("attitude_neutral", details.getAttitude());
	ASSERT_EQ(0, details.getRelations());
	ASSERT_FALSE(details.hasMilitaryAccess());
}

TEST(EU4World_Relations_RelationDetailsTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "attitude = attitude_rival\n";
	input << "value = -137\n";
	input << "military_access = irrelevant\n";
	const EU4::EU4RelationDetails details(input);

	ASSERT_EQ("attitude_rival", details.getAttitude());
	ASSERT_EQ(-137, details.getRelations());
	ASSERT_TRUE(details.hasMilitaryAccess());
}
