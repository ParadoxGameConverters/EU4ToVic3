#include "Loaders/VanillaBuildingLoader/VanillaBuildingLoader.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_VanillaBuildingLoaderTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaBuildingLoader loader;

	EXPECT_TRUE(loader.getBuildingElements().empty());
}

TEST(V3World_VanillaBuildingLoaderTests, VanillaBuildingsCanBeLoaded)
{
	V3::VanillaBuildingLoader loader;
	loader.loadVanillaBuildings(modFS);

	EXPECT_EQ(3, loader.getBuildingElements().size());
	EXPECT_EQ(1, loader.getBuildingElements().at("STATE_SVEALAND").size());
	EXPECT_EQ(1, loader.getBuildingElements().at("STATE_GOTALAND").size());
	EXPECT_EQ(2, loader.getBuildingElements().at("STATE_LOMBARDY").size());

	EXPECT_EQ(2, loader.getBuildingElements().at("STATE_SVEALAND").at("SWE").size());
	EXPECT_EQ(1, loader.getBuildingElements().at("STATE_GOTALAND").at("SWE").size());
	EXPECT_EQ(1, loader.getBuildingElements().at("STATE_LOMBARDY").at("AUS").size());
	EXPECT_EQ(1, loader.getBuildingElements().at("STATE_LOMBARDY").at("SAR").size());
}
