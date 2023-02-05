#include "Loaders/VanillaDiplomacyLoader/VanillaDiplomacyLoader.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_VanillaDiplomacyLoaderTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaDiplomacyLoader loader;

	EXPECT_TRUE(loader.getAgreementEntries().empty());
	EXPECT_TRUE(loader.getRelationEntries().empty());
	EXPECT_TRUE(loader.getTruceEntries().empty());
}

TEST(V3World_VanillaDiplomacyLoaderTests, VanillaAgreementsCanBeLoaded)
{
	V3::VanillaDiplomacyLoader loader;
	loader.loadVanillaDiplomacy(modFS);

	EXPECT_EQ(13, loader.getAgreementEntries().size());
	EXPECT_EQ(5, loader.getRelationEntries().size());
	EXPECT_EQ(3, loader.getTruceEntries().size());
}
