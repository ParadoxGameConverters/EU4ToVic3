#include "EconomyManager/Building/Building.h"
#include "Loaders/BuildingLoader/BuildingLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game", {});
}

TEST(V3World_BuildingLoaderTests, BuildingLoaderCanLoadBuildings)
{
	V3::BuildingLoader buildingLoader;
	EXPECT_TRUE(buildingLoader.getBuildings().empty());
	buildingLoader.loadBuildings(modFS);

	const auto buildings = buildingLoader.getBuildings();
	EXPECT_EQ(3, buildings.size());

	EXPECT_THAT("bg_light_industry", buildings.at("building_food_industry").getBuildingGroup());
	EXPECT_TRUE(buildings.at("building_food_industry").getUnlockingTechs().contains("manufacturies"));
	EXPECT_EQ(300, buildings.at("building_food_industry").getConstructionCost());
	EXPECT_THAT(buildings.at("building_food_industry").getPMGroups(),
		 testing::UnorderedElementsAre("pmg_food", "pmg_canning", "pmg_distillery", "pmg_automation", "pmg_ownership"));

	EXPECT_EQ(239, buildings.at("building_textile_mill").getConstructionCost());
	EXPECT_THAT(buildings.at("building_textile_mill").getPMGroups(),
		 testing::UnorderedElementsAre("pmg_textile", "pmg_luxury", "pmg_automation", "pmg_ownership"));

	EXPECT_EQ(50, buildings.at("building_furniture_manufactory").getConstructionCost());
	EXPECT_THAT(buildings.at("building_furniture_manufactory").getPMGroups(),
		 testing::UnorderedElementsAre("pmg_furniture", "pmg_luxury", "pmg_automation", "pmg_ownership"));
}