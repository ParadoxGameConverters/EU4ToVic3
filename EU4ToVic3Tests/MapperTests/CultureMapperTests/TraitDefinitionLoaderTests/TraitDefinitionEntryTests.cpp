#include "CultureMapper/TraitDefinitionLoader/TraitDefinitionEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_TraitDefinitionEntryTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;
	const mappers::TraitDefinitionEntry entry(input);

	EXPECT_FALSE(entry.getTraitDef().isHeritageTrait);
}

TEST(Mappers_TraitDefinitionEntryTests, EntryCanBeLoaded)
{
	std::stringstream input;
	input << "heritage = yes\n";
	const mappers::TraitDefinitionEntry entry(input);

	EXPECT_TRUE(entry.getTraitDef().isHeritageTrait);
}
