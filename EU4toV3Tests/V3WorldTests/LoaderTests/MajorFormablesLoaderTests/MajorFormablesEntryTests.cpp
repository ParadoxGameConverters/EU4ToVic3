#include "Loaders/MajorFormablesLoader/MajorFormablesEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_MajorFormablesEntryTests, DefaultsDefaultToDefaults)
{
	const V3::MajorFormablesEntry entry;

	EXPECT_TRUE(entry.getStanzas().empty());
	EXPECT_TRUE(entry.getCultures().empty());
	EXPECT_TRUE(entry.getRequiredTechnology().empty());
}

TEST(V3World_MajorFormablesEntryTests, FormableCanBeLoaded)
{
	std::stringstream input;
	input << "stanza = something\n";
	input << "stanza2 = { something else }\n";
	input << "possible = {\n";
	input << "	OR = {\n";
	input << "		not really relevant\n";
	input << "	}\n";
	input << "	any_country = {\n";
	input << "		OR = {\n ";
	input << "			country_has_primary_culture = cu:cul1\n";
	input << "			country_has_primary_culture = cu:cul2\n";
	input << "		}\n";
	input << "		has_technology_researched = theTechnology\n";
	input << "	}\n";
	input << "}\n";
	const V3::MajorFormablesEntry entry(input);

	ASSERT_EQ(2, entry.getStanzas().size());
	EXPECT_EQ("stanza = something", entry.getStanzas()[0]);
	EXPECT_EQ("stanza2 = { something else }", entry.getStanzas()[1]);
	EXPECT_THAT(entry.getCultures(), testing::UnorderedElementsAre("cul1", "cul2"));
	EXPECT_EQ("theTechnology", entry.getRequiredTechnology());
}
