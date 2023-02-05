#include "CultureMapper/NameListLoader/NameListEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_NameListEntryTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;
	const mappers::NameListEntry entry(input);

	EXPECT_TRUE(entry.getDynastyNames().empty());
	EXPECT_TRUE(entry.getMaleNames().empty());
	EXPECT_TRUE(entry.getFemaleNames().empty());
}

TEST(Mappers_NameListEntryTests, EntryCanBeLoaded)
{
	std::stringstream input;
	input << "dynasty_names = { dyn1 dyn2 }\n";
	input << "male_names = { male1 male2 }\n";
	input << "female_names = { female1 female2 }\n";
	const mappers::NameListEntry entry(input);

	EXPECT_THAT(entry.getDynastyNames(), testing::UnorderedElementsAre("dyn1", "dyn2"));
	EXPECT_THAT(entry.getMaleNames(), testing::UnorderedElementsAre("male1", "male2"));
	EXPECT_THAT(entry.getFemaleNames(), testing::UnorderedElementsAre("female1", "female2"));
}
