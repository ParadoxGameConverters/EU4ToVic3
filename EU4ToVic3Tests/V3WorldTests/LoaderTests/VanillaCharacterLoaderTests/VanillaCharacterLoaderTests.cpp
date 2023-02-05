#include "Loaders/VanillaCharacterLoader/VanillaCharacterLoader.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_VanillaCharacterLoaderTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaCharacterLoader loader;

	EXPECT_TRUE(loader.getCharacterElements().empty());
}

TEST(V3World_VanillaCharacterLoaderTests, VanillaCharactersCanBeLoaded)
{
	V3::VanillaCharacterLoader loader;
	loader.loadVanillaCharacters(modFS);

	EXPECT_EQ(2, loader.getCharacterElements().size());
	EXPECT_EQ(2, loader.getCharacterElements().at("ABU").size());
	EXPECT_EQ(2, loader.getCharacterElements().at("ACE").size());
}
