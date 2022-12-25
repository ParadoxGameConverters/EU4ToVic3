#include "Loaders/BuildingLoader/ProductionMethodLoader/ProductionMethodGroupLoader.h"
#include "ModLoader/ModFilesystem.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
TEST(V3World_ProductionMethodGroupLoaderTests, ProductionMethodGroupLoaderCanLoadProductionMethodGroups)
{
	V3::ProductionMethodGroupLoader PMGroupLoader;
	const auto PMGroups = PMGroupLoader.getPMGroups();
	EXPECT_TRUE(PMGroups.empty());
	PMGroupLoader.loadPMGroups(modFS);
	EXPECT_EQ(6, PMGroups.size());
}