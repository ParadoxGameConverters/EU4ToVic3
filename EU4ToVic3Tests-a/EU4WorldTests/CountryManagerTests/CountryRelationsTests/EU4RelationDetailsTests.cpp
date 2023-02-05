#include "CountryManager/CountryRelations/EU4RelationDetails.h"
#include "gtest/gtest.h"

TEST(EU4World_RelationDetailsTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const EU4::EU4RelationDetails details(input);

	EXPECT_EQ("attitude_neutral", details.getAttitude());
	EXPECT_EQ(0, details.getRelations());
	EXPECT_FALSE(details.hasMilitaryAccess());
	EXPECT_FALSE(details.getTruceExpiry());
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
	EXPECT_FALSE(details.getTruceExpiry());
}

TEST(EU4World_RelationDetailsTests, truceExpiryCanBeCalculated)
{
	std::stringstream input;
	input << "last_war = \"1000.1.1\"\n";
	input << "last_warscore = 100\n";
	input << "truce = yes\n";
	const EU4::EU4RelationDetails details(input);

	ASSERT_TRUE(details.getTruceExpiry());
	EXPECT_EQ(date("1015.1.1"), *details.getTruceExpiry());
}

TEST(EU4World_RelationDetailsTests, truceExpiryLastsFiveYearsForDiplobreak)
{
	std::stringstream input;
	input << "last_war = \"1000.1.1\"\n";
	input << "truce = yes\n";
	const EU4::EU4RelationDetails details(input);

	ASSERT_TRUE(details.getTruceExpiry());
	EXPECT_EQ(date("1005.1.1"), *details.getTruceExpiry());
}
