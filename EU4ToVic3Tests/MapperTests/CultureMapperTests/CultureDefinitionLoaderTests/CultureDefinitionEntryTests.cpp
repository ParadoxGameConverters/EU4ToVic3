#include "CultureMapper/CultureDefinitionLoader/CultureDefinitionEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_CultureDefinitionEntryTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;
	const mappers::CultureDefinitionEntry entry(input, false, false);

	EXPECT_TRUE(entry.getCultureDef().name.empty());
	EXPECT_FALSE(entry.getCultureDef().color);
	EXPECT_TRUE(entry.getCultureDef().religion.empty());
	EXPECT_TRUE(entry.getCultureDef().traits.empty());
	EXPECT_TRUE(entry.getCultureDef().maleCommonFirstNames.empty());
	EXPECT_TRUE(entry.getCultureDef().femaleCommonFirstNames.empty());
	EXPECT_TRUE(entry.getCultureDef().nobleLastNames.empty());
	EXPECT_TRUE(entry.getCultureDef().commonLastNames.empty());
	EXPECT_TRUE(entry.getCultureDef().maleRegalFirstNames.empty());
	EXPECT_TRUE(entry.getCultureDef().femaleRegalFirstNames.empty());
	EXPECT_TRUE(entry.getCultureDef().regalLastNames.empty());
	EXPECT_TRUE(entry.getCultureDef().ethnicities.empty());
	EXPECT_TRUE(entry.getCultureDef().graphics.empty());
	EXPECT_FALSE(entry.getCultureDef().skipProcessing);
	EXPECT_FALSE(entry.getCultureDef().skipExport);
}

TEST(Mappers_CultureDefinitionEntryTests, EntryCanBeLoaded)
{
	std::stringstream input;
	input << "color = rgb{ 1 2 3 }\n";
	input << "religion = religion_1\n";
	input << "traits = { testtrait1 }\n";
	input << "male_common_first_names= { mcfn1 mcfn2 }\n";
	input << "female_common_first_names = { fcfn1 fcfn2 }\n";
	input << "noble_last_names = { nln1 nln2 }\n";
	input << "common_last_names={ cln1 cln2 }\n";
	input << "male_regal_first_names = { mrfn1 mrfn2 }\n";
	input << "female_regal_first_names = { frfn1 frfn2 }\n";
	input << "regal_last_names = { rln1 rln2 }\n";
	input << "ethnicities = {\n";
	input << "	1 = ethnotest\n";
	input << "}\n";
	input << "graphics = graphtest\n";
	const mappers::CultureDefinitionEntry entry(input, true, true);

	EXPECT_TRUE(entry.getCultureDef().name.empty());
	EXPECT_EQ(commonItems::Color(std::array{1, 2, 3}), *entry.getCultureDef().color);
	EXPECT_THAT(entry.getCultureDef().traits, testing::UnorderedElementsAre("testtrait1"));
	EXPECT_THAT(entry.getCultureDef().maleCommonFirstNames, testing::UnorderedElementsAre("mcfn1", "mcfn2"));
	EXPECT_THAT(entry.getCultureDef().femaleCommonFirstNames, testing::UnorderedElementsAre("fcfn1", "fcfn2"));
	EXPECT_THAT(entry.getCultureDef().nobleLastNames, testing::UnorderedElementsAre("nln1", "nln2"));
	EXPECT_THAT(entry.getCultureDef().commonLastNames, testing::UnorderedElementsAre("cln1", "cln2"));
	EXPECT_THAT(entry.getCultureDef().maleRegalFirstNames, testing::UnorderedElementsAre("mrfn1", "mrfn2"));
	EXPECT_THAT(entry.getCultureDef().femaleRegalFirstNames, testing::UnorderedElementsAre("frfn1", "frfn2"));
	EXPECT_THAT(entry.getCultureDef().regalLastNames, testing::UnorderedElementsAre("rln1", "rln2"));
	EXPECT_THAT(entry.getCultureDef().ethnicities, testing::UnorderedElementsAre("ethnotest"));
	EXPECT_EQ("graphtest", entry.getCultureDef().graphics);
	EXPECT_TRUE(entry.getCultureDef().skipProcessing);
	EXPECT_TRUE(entry.getCultureDef().skipExport);
}
