#include "Loaders/LawLoader/LawEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_LawEntryTests, DefaultsDefaultToDefaults)
{
	const V3::LawEntry entry;
	const auto& law = entry.getLaw();

	EXPECT_TRUE(law.group.empty());
	EXPECT_EQ(0, law.progressiveness);
	EXPECT_TRUE(law.requiredTechs.empty());
	EXPECT_TRUE(law.requiredLaws.empty());
	EXPECT_TRUE(law.blockingLaws.empty());
}

TEST(V3World_LawEntryTests, LawCanBeLoaded)
{
	std::stringstream input;
	input << "group = lawGroup\n ";
	input << "progressiveness = -89\n ";
	input << "unlocking_technologies = {\n";
	input << "	tech1 tech2 tech3\n";
	input << "}\n";
	input << "unlocking_laws = {\n";
	input << "	u1 u2 u3\n";
	input << "}\n ";
	input << "disallowing_laws = {\n ";
	input << "	b1 b2 b3\n";
	input << "}\n ";
	input << "institution = i1\n";
	input << "modifier = {\n ";
	input << "\tstate_bureaucracy_population_base_cost_factor_mult = -0.14\n";
	input << "\tstate_building_barracks_max_level_add = 30\n";
	input << "}\n ";
	const V3::LawEntry entry(input);
	const auto& law = entry.getLaw();

	EXPECT_EQ("lawGroup", law.group);
	EXPECT_EQ(-89, law.progressiveness);
	EXPECT_THAT(law.requiredTechs, testing::UnorderedElementsAre("tech1", "tech2", "tech3"));
	EXPECT_THAT(law.requiredLaws, testing::UnorderedElementsAre("u1", "u2", "u3"));
	EXPECT_THAT(law.blockingLaws, testing::UnorderedElementsAre("b1", "b2", "b3"));
	EXPECT_EQ("i1", law.institution);
	EXPECT_EQ(-0.14, law.bureaucracyCostMult);
	EXPECT_EQ(30, law.maxBuildingLevels.at("building_barracks"));
}
