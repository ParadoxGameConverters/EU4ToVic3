#include "CultureLoader/CultureGroupParser.h"
#include "CultureLoader/CultureLoader.h"
#include "gtest/gtest.h"

TEST(EU4World_CultureLoaderTests, cultureGroupsCanBeLoaded)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	EU4::CultureLoader groups;
	groups.loadCultures(input);

	EXPECT_EQ(2, groups.getCultureGroupsMap().size());
	EXPECT_TRUE(groups.getCultureGroupsMap().contains("groupA"));
	EXPECT_TRUE(groups.getCultureGroupsMap().contains("groupB"));
}

TEST(EU4World_CultureLoaderTests, excessCulturesAreSkipped)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureA = {} }\n";
	EU4::CultureLoader groups;
	groups.loadCultures(input);

	EXPECT_EQ(2, groups.getCultureGroupsMap().size());
	EXPECT_TRUE(groups.getCultureGroupsMap().contains("groupA"));
	EXPECT_TRUE(groups.getCultureGroupsMap().at("groupA").getCultureNames().contains("cultureA"));
	EXPECT_TRUE(groups.getCultureGroupsMap().contains("groupB"));
	EXPECT_FALSE(groups.getCultureGroupsMap().at("groupB").getCultureNames().contains("cultureA"));
}

TEST(EU4World_CultureLoaderTests, GroupNameForCultureCanBeRetrieved)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	EU4::CultureLoader groups;
	groups.loadCultures(input);

	EXPECT_EQ("groupA", groups.getGroupNameForCulture("cultureA"));
	EXPECT_EQ("groupB", groups.getGroupNameForCulture("cultureB"));
}

TEST(EU4World_CultureLoaderTests, GroupNameForCultureReturnsNulloptForFailure)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	EU4::CultureLoader groups;
	groups.loadCultures(input);

	EXPECT_EQ(std::nullopt, groups.getGroupNameForCulture("cultureC"));
}

TEST(EU4World_CultureLoaderTests, GroupForCultureCanBeRetrieved)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	EU4::CultureLoader groups;
	groups.loadCultures(input);

	EXPECT_EQ("groupA", groups.getGroupForCulture("cultureA")->getName());
	EXPECT_EQ("groupB", groups.getGroupForCulture("cultureB")->getName());
}

TEST(EU4World_CultureLoaderTests, GroupForCultureReturnsNulloptForFailure)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	EU4::CultureLoader groups;
	groups.loadCultures(input);

	EXPECT_EQ(std::nullopt, groups.getGroupForCulture("cultureC"));
}

TEST(EU4World_CultureLoaderTests, CultureCanBePinged)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	EU4::CultureLoader groups;
	groups.loadCultures(input);

	EXPECT_TRUE(groups.containsCulture("cultureA"));
	EXPECT_TRUE(groups.containsCulture("cultureB"));
	EXPECT_FALSE(groups.containsCulture("cultureC"));
}

TEST(EU4World_CultureLoaderTests, mergableCultureGroupsCanBeMerged)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	EU4::CultureLoader groups;
	groups.loadCultures(input);

	std::stringstream input2;
	input2 << "groupA = { cultureC = {} }\n";
	input2 << "groupB = { cultureD = {} }\n";
	groups.loadCultures(input2);

	EXPECT_EQ(2, groups.getCultureGroupsMap().size());
	EXPECT_TRUE(groups.getCultureGroupsMap().contains("groupA"));
	EXPECT_TRUE(groups.getCultureGroupsMap().contains("groupB"));

	const auto& cultureGroupA = groups.getCultureGroupsMap().at("groupA");
	EXPECT_EQ(2, cultureGroupA.getCultures().size());
	EXPECT_TRUE(cultureGroupA.getCultures().contains("cultureA"));
	EXPECT_TRUE(cultureGroupA.getCultures().contains("cultureC"));

	const auto& cultureGroupB = groups.getCultureGroupsMap().at("groupB");
	EXPECT_EQ(2, cultureGroupB.getCultures().size());
	EXPECT_TRUE(cultureGroupB.getCultures().contains("cultureB"));
	EXPECT_TRUE(cultureGroupB.getCultures().contains("cultureD"));
}
