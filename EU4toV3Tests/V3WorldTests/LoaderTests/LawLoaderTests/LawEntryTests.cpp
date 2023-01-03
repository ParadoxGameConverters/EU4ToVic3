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
	EXPECT_TRUE(law.reqiredLaws.empty());
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
	const V3::LawEntry entry(input);
	const auto& law = entry.getLaw();

	EXPECT_EQ("lawGroup", law.group);
	EXPECT_EQ(-89, law.progressiveness);
	EXPECT_THAT(law.requiredTechs, testing::UnorderedElementsAre("tech1", "tech2", "tech3"));
	EXPECT_THAT(law.reqiredLaws, testing::UnorderedElementsAre("u1", "u2", "u3"));
	EXPECT_THAT(law.blockingLaws, testing::UnorderedElementsAre("b1", "b2", "b3"));
}
