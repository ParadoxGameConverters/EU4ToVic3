#include "CountryManager/CountryRelations/EU4RelationDetails.h"
#include "gtest/gtest.h"

TEST(EU4World_RelationDetailsTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const EU4::EU4RelationDetails details(input);

	EXPECT_EQ("attitude_neutral", details.getAttitude());
	EXPECT_EQ(0, details.getRelations());
	EXPECT_FALSE(details.hasMilitaryAccess());
}

TEST(EU4World_RelationDetailsTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "attitude = attitude_rival\n";
	input << "cached_sum = -137\n";
	input << "military_access = irrelevant\n";
	const EU4::EU4RelationDetails details(input);

	EXPECT_EQ("attitude_rival", details.getAttitude());
	EXPECT_EQ(-137, details.getRelations());
	EXPECT_TRUE(details.hasMilitaryAccess());
}
