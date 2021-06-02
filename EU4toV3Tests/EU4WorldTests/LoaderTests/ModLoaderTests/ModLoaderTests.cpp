#include "ModLoader/ModLoader.h"
#include "gtest/gtest.h"
#include "OSCompatibilityLayer.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;
using testing::Pair;

TEST(EU4World_ModLoaderTests, ModsCanBeLocatedUnpackedAndUpdated)
{
	std::stringstream configurationInput;
	configurationInput << "EU4DocumentsDirectory = \"TestFiles\"\n";
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	const auto configuration = Configuration(configurationInput);

	EU4::Mods mods; // this is what comes from the save
	mods.emplace("Some mod", "mod/themod.mod"); // mod's in fact named "The Mod" in the file.

	EU4::ModLoader modLoader;
	modLoader.loadMods(configuration, mods);
	mods = modLoader.getMods();

	EXPECT_THAT(mods, UnorderedElementsAre(Pair("The Mod", "TestFiles/mod/themod/")));
}

TEST(EU4World_ModLoaderTests, BokenMissingAndNonexistentModsAreDiscarded)
{
	std::stringstream configurationInput;
	configurationInput << "EU4DocumentsDirectory = \"TestFiles\"\n";
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	const auto configuration = Configuration(configurationInput);

	EU4::Mods mods;
	mods.emplace("Some mod", "mod/themod.mod");
	mods.emplace("Broken mod", "mod/brokenmod.mod"); // no path
	mods.emplace("Missing mod", "mod/missingmod.mod"); // missing directory
	mods.emplace("Nonexistent mod", "mod/nonexistentmod.mod"); // doesn't exist.

	EU4::ModLoader modLoader;
	modLoader.loadMods(configuration, mods);
	mods = modLoader.getMods();

	EXPECT_THAT(mods, UnorderedElementsAre(Pair("The Mod", "TestFiles/mod/themod/")));
}

TEST(EU4World_ModLoaderTests, CompressedModsCanBeUnpacked)
{
	std::stringstream configurationInput;
	configurationInput << "EU4DocumentsDirectory = \"TestFiles\"\n";
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	const auto configuration = Configuration(configurationInput);

	EU4::Mods mods;
	mods.emplace("some packed mod", "mod/packedmod.mod");

	EU4::ModLoader modLoader;
	modLoader.loadMods(configuration, mods);
	mods = modLoader.getMods();

	EXPECT_THAT(mods, UnorderedElementsAre(Pair("Packed Mod", "mods/packedmod/")));
	EXPECT_TRUE(commonItems::DoesFolderExist("mods/packedmod/"));
}
