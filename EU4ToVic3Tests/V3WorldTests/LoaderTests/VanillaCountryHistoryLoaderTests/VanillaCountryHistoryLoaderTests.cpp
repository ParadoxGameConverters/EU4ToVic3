#include "Loaders/VanillaCountryHistoryLoader/VanillaCountryHistoryLoader.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_VanillaCountryHistoryLoaderTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaCountryHistoryLoader loader;

	EXPECT_TRUE(loader.getCountryHistoryElements().empty());
}

TEST(V3World_VanillaCountryHistoryLoaderTests, VanillaCountryHistoriesCanBeLoaded)
{
	V3::VanillaCountryHistoryLoader loader;
	loader.loadVanillaCountryHistories(modFS);

	EXPECT_EQ(2, loader.getCountryHistoryElements().size());
	EXPECT_EQ(26, loader.getCountryHistoryElements().at("USA").size());
	EXPECT_EQ(3, loader.getCountryHistoryElements().at("UTE").size());
}
