#include "ReligionMapper/ReligionDefinitionLoader/ReligionDefinitionEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ReligionDefinitionEntryTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;
	const mappers::ReligionDefinitionEntry entry(input);

	EXPECT_TRUE(entry.getReligionDef().name.empty());
	EXPECT_TRUE(entry.getReligionDef().texture.empty());
	EXPECT_TRUE(entry.getReligionDef().traits.empty());
	EXPECT_FALSE(entry.getReligionDef().color);
	EXPECT_TRUE(entry.getReligionDef().taboos.empty());
}

TEST(Mappers_ReligionDefinitionEntryTests, EntryCanBeLoaded)
{
	std::stringstream input;
	input << R"(texture = "gfx/interface/icons/religion_icons/shiite.dds")";
	input << R"(traits = {)";
	input << R"(	muslim)";
	input << R"(})";
	input << R"(color = { 0.4 0.7 0.3 })";
	input << R"(taboos = {)";
	input << R"(	liquor wine)";
	input << R"(})";
	const mappers::ReligionDefinitionEntry entry(input);

	EXPECT_TRUE(entry.getReligionDef().name.empty());
	EXPECT_EQ("gfx/interface/icons/religion_icons/shiite.dds", entry.getReligionDef().texture);
	EXPECT_THAT(entry.getReligionDef().traits, testing::UnorderedElementsAre("muslim"));
	EXPECT_EQ(commonItems::Color(std::array{102, 179, 77}), *entry.getReligionDef().color);
	EXPECT_THAT(entry.getReligionDef().taboos, testing::UnorderedElementsAre("liquor", "wine"));
}
