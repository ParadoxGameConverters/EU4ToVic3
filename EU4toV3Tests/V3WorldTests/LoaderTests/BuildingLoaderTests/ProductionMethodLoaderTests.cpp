#include "Loaders/BuildingLoader/ProductionMethodLoader/ProductionMethodLoader.h"
#include "ModLoader/ModFilesystem.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
TEST(V3World_ProductionMethodLoaderTests, ProductionMethodLoaderCanLoadProductionMethods)
{
	V3::ProductionMethodLoader PMLoader;
	const auto PMs = PMLoader.getPMs();
	EXPECT_TRUE(PMs.empty());
	PMLoader.loadPMs(modFS);
	EXPECT_EQ(6, PMs.size());
}