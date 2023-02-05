#include "ReligionMapper/ReligionGroupMapper/ReligionGroupMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ReligionGroupMappingTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;
	const mappers::ReligionGroupMapping mapping(input);

	EXPECT_TRUE(mapping.getTrait().empty());
	EXPECT_TRUE(mapping.getEU4Groups().empty());
	EXPECT_TRUE(mapping.getTaboos().empty());
	EXPECT_TRUE(mapping.getIcon().empty());
}

TEST(Mappers_ReligionGroupMappingTests, MappingCanBeLoaded)
{
	std::stringstream input;
	input << "vic3 = trait eu4 = religious_group taboo = some_taboo taboo = other_taboo icon = religion";
	const mappers::ReligionGroupMapping mapping(input);

	EXPECT_EQ("trait", mapping.getTrait());
	EXPECT_THAT(mapping.getEU4Groups(), testing::UnorderedElementsAre("religious_group"));
	EXPECT_THAT(mapping.getTaboos(), testing::UnorderedElementsAre("some_taboo", "other_taboo"));
	EXPECT_EQ("religion", mapping.getIcon());
}
