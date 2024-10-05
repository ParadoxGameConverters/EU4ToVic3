#include "Loaders/BuildingLoader/ProductionMethodLoader/ProductionMethodLoader.h"
#include "ModLoader/ModFilesystem.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_ProductionMethodLoaderTests, ProductionMethodLoaderCanLoadProductionMethods)
{
	V3::ProductionMethodLoader PMLoader;
	EXPECT_TRUE(PMLoader.getPMs().empty());
	PMLoader.loadPMs(modFS);
	const auto PMs = PMLoader.getPMs();
	EXPECT_EQ(24, PMs.size());
}