#include "Loaders/BuildingLoader/ProductionMethodLoader/ProductionMethodGroupLoader.h"
#include "ModLoader/ModFilesystem.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
TEST(V3World_ProductionMethodGroupLoaderTests, ProductionMethodGroupLoaderCanLoadProductionMethodGroups)
{
	V3::ProductionMethodGroupLoader PMGroupLoader;
	EXPECT_TRUE(PMGroupLoader.getPMGroups().empty());
	PMGroupLoader.loadPMGroups(modFS);
	const auto PMGroups = PMGroupLoader.getPMGroups();
	EXPECT_EQ(8, PMGroups.size());
}