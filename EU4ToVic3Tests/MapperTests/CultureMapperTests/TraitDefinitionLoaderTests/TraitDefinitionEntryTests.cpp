#include "CultureMapper/TraitDefinitionLoader/TraitDefinitionEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_TraitDefinitionEntryTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;
	const mappers::TraitDefinitionEntry entry(input);

	EXPECT_FALSE(entry.getTraitDef().isHeritage);
	EXPECT_FALSE(entry.getTraitDef().isLanguage);
	EXPECT_FALSE(entry.getTraitDef().isTradition);
}

TEST(Mappers_TraitDefinitionEntryTests, EntryCanBeLoaded)
{
	std::stringstream input;
	input << "heritage = yes\n";
	input << "language = yes\n";
	input << "tradition = yes\n";
	const mappers::TraitDefinitionEntry entry(input);

	EXPECT_TRUE(entry.getTraitDef().isHeritage);
	EXPECT_TRUE(entry.getTraitDef().isLanguage);
	EXPECT_TRUE(entry.getTraitDef().isTradition);
}
