#include "Loaders/DefinesLoader/BuildingScriptValuesLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
TEST(V3World_BuildingScriptValuesLoaderTests, CostTiersCanBeLoaded)
{
	V3::BuildingScriptValuesLoader buildingScriptValuesLoader;
	const auto costTiers = buildingScriptValuesLoader.getBuildingCostConstants();
	EXPECT_TRUE(costTiers.empty());
	buildingScriptValuesLoader.loadBuildingScriptValues(modFS);
	EXPECT_EQ(3, costTiers.size());

	EXPECT_THAT(costTiers,
		 testing::UnorderedElementsAre(std::pair("construction_cost_very_low", 50),
			  std::pair("construction_cost_low", 100),
			  std::pair("construction_cost_medium", 200)));
}