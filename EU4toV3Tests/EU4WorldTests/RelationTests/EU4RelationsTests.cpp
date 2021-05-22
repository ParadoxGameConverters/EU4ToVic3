#include "Relations/EU4Relations.h"
#include "gtest/gtest.h"

TEST(EU4World_Relations_RelationsTests, relationsDefaultToEmpty)
{
	std::stringstream input;
	const EU4::EU4Relations relations(input);

	ASSERT_TRUE(relations.getRelations().empty());
}

TEST(EU4World_Relations_RelationsTests, relationsCanBeLoaded)
{
	std::stringstream input;
	input << "HAB = {\n";
	input << "\tvalue = -137\n";
	input << "\tmilitary_access = irrelevant\n";
	input << "}\n";
	input << "BOH = {\n";
	input << "\tattitude = attitude_rival\n";
	input << "}\n";
	const EU4::EU4Relations relations(input);

	const auto hab = relations.getRelations().find("HAB")->second;
	const auto boh = relations.getRelations().find("BOH")->second;

	ASSERT_EQ("attitude_neutral", hab.getAttitude());
	ASSERT_EQ(-137, hab.getRelations());
	ASSERT_TRUE(hab.hasMilitaryAccess());
	ASSERT_EQ("attitude_rival", boh.getAttitude());
	ASSERT_EQ(0, boh.getRelations());
	ASSERT_FALSE(boh.hasMilitaryAccess());
}
