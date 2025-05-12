#include "Loaders/VanillaDiplomaticPlayLoader/VanillaDiplomaticPlayLoader.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_VanillaDiplomaticPlayLoaderTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaDiplomaticPlayLoader loader;

	EXPECT_TRUE(loader.getDiploPlayElements().empty());
}

TEST(V3World_VanillaDiplomaticPlayLoaderTests, VanillaDiplomaticPlaysCanBeLoaded)
{
	V3::VanillaDiplomaticPlayLoader loader;
	loader.loadVanillaDiplomaticPlays(modFS);

	EXPECT_EQ(2, loader.getDiploPlayElements().size());
	EXPECT_EQ(2, loader.getDiploPlayElements().at("BRZ").size());
	EXPECT_EQ(1, loader.getDiploPlayElements().at("TEX").size());
}
