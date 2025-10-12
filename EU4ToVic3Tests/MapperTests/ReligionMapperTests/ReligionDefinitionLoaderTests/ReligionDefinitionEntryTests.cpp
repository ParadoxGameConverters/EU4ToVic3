#include "ReligionMapper/ReligionDefinitionLoader/ReligionDefinitionEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_ReligionDefinitionEntryTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;
	const mappers::ReligionDefinitionEntry entry(input);

	EXPECT_TRUE(entry.getReligionDef().name.empty());
	EXPECT_TRUE(entry.getReligionDef().texture.empty());
	EXPECT_TRUE(entry.getReligionDef().heritage.empty());
	EXPECT_FALSE(entry.getReligionDef().color);
	EXPECT_TRUE(entry.getReligionDef().taboos.empty());
}

TEST(Mappers_ReligionDefinitionEntryTests, EntryCanBeLoaded)
{
	std::stringstream input;
	input << "icon = \"gfx/interface/icons/religion_icons/shiite.dds\"\n";
	input << "heritage = muslim\n";
	input << "color = { 0.4 0.7 0.3 }\n";
	input << "taboos = {\n";
	input << "	liquor wine\n";
	input << "}";
	const mappers::ReligionDefinitionEntry entry(input);

	EXPECT_TRUE(entry.getReligionDef().name.empty());
	EXPECT_EQ("gfx/interface/icons/religion_icons/shiite.dds", entry.getReligionDef().texture);
	EXPECT_EQ("muslim", entry.getReligionDef().heritage);
	EXPECT_EQ(commonItems::Color(std::array{102, 179, 77}), *entry.getReligionDef().color);
	EXPECT_THAT(entry.getReligionDef().taboos, testing::UnorderedElementsAre("liquor", "wine"));
}
