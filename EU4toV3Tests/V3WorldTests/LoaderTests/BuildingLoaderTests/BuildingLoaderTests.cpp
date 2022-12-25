#include "Loaders/BuildingLoader/BuildingLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

#include "EconomyManager/Building/Building.h"

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
TEST(V3World_BuildingLoaderTests, BuildingLoaderCanLoadBuildings)
{
	V3::BuildingLoader buildingLoader;
	const auto buildings = buildingLoader.getBuildings();
	EXPECT_TRUE(buildings.empty());
	buildingLoader.loadBuildings(modFS);
	EXPECT_EQ(3, buildings.size());

	EXPECT_THAT("light_industry", buildings.at("building_food_industry")->getBuildingGroup());
	EXPECT_THAT("manufacturies", buildings.at("building_food_industry")->getPreReqTech());
	EXPECT_EQ(200, buildings.at("building_food_industry")->getConstructionCost());
	EXPECT_THAT(buildings.at("building_food_industry")->getPMGroups(),
		 testing::UnorderedElementsAre("pmg_food", "pmg_canning", "pmg_distillery", "pmg_automation", "pmg_ownership"));

	EXPECT_EQ(239, buildings.at("building_textile_mills")->getConstructionCost());
	EXPECT_THAT(buildings.at("building_textile_mills")->getPMGroups(),
		 testing::UnorderedElementsAre("pmg_textile", "pmg_luxury", "pmg_automation", "pmg_ownership"));

	EXPECT_EQ(50, buildings.at("building_furniture_manufacturies")->getConstructionCost());
	EXPECT_THAT(buildings.at("building_furniture_manufacturies")->getPMGroups(),
		 testing::UnorderedElementsAre("pmg_furniture", "pmg_luxury", "pmg_automation", "pmg_ownership"));
}