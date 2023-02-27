#include "Loaders/VanillaPopulationHistoryLoader/VanillaPopulationHistoryLoader.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_VanillaPopulationHistoryLoaderTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaPopulationHistoryLoader loader;

	EXPECT_TRUE(loader.getPopulationHistoryElements().empty());
}

TEST(V3World_VanillaPopulationHistoryLoaderTests, VanillaPopulationHistoriesCanBeLoaded)
{
	V3::VanillaPopulationHistoryLoader loader;
	loader.loadVanillaPopulationHistories(modFS);

	EXPECT_EQ(2, loader.getPopulationHistoryElements().size());
	EXPECT_EQ(2, loader.getPopulationHistoryElements().at("RUS").size());
	EXPECT_EQ(2, loader.getPopulationHistoryElements().at("KRA").size());
}
