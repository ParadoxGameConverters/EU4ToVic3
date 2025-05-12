#include "Loaders/DefinesLoader/BuildingScriptValuesLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_BuildingScriptValuesLoaderTests, CostTiersCanBeLoaded)
{
	V3::BuildingScriptValuesLoader buildingScriptValuesLoader;
	EXPECT_TRUE(buildingScriptValuesLoader.getBuildingCostConstants().empty());
	buildingScriptValuesLoader.loadBuildingScriptValues(modFS);
	const auto costTiers = buildingScriptValuesLoader.getBuildingCostConstants();
	EXPECT_EQ(4, costTiers.size());

	EXPECT_THAT(costTiers,
		 testing::UnorderedElementsAre(std::pair("construction_cost_very_low", 50),
			  std::pair("construction_cost_low", 150),
			  std::pair("construction_cost_medium", 300),
			  std::pair("construction_cost_monument", 2500)));
}