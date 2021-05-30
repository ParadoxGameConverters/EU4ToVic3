#include "CountryManager/CountryRelations/EU4Relations.h"
#include "gtest/gtest.h"

TEST(EU4World_RelationsTests, relationsDefaultToEmpty)
{
	std::stringstream input;
	const EU4::EU4Relations relations(input);

	EXPECT_TRUE(relations.getRelations().empty());
}

TEST(EU4World_RelationsTests, relationsCanBeLoaded)
{
	std::stringstream input;
	input << "HAB = {\n";
	input << "\tcached_sum = -137\n";
	input << "\tmilitary_access = irrelevant\n";
	input << "}\n";
	input << "BOH = {\n";
	input << "\tattitude = attitude_rival\n";
	input << "}\n";
	const EU4::EU4Relations relations(input);

	const auto habRelation = relations.getRelations().at("HAB");
	const auto bohRelation = relations.getRelations().at("BOH");

	EXPECT_EQ("attitude_neutral", habRelation.getAttitude());
	EXPECT_EQ(-137, habRelation.getRelations());
	EXPECT_TRUE(habRelation.hasMilitaryAccess());

	EXPECT_EQ("attitude_rival", bohRelation.getAttitude());
	EXPECT_EQ(0, bohRelation.getRelations());
	EXPECT_FALSE(bohRelation.hasMilitaryAccess());
}
