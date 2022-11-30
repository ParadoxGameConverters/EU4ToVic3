#include "EconomyManager/BuildingLoader/BuildingGroupLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_CountryDefinitionLoaderTests, BuildingGroupLoaderCanLoadBuildingGroups)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	auto buildingGroups = buildingGroupLoader.getBuildingGroups();
	EXPECT_EQ(buildingGroups->getBuildingGroupMap().size(), 0);
	buildingGroupLoader.loadBuildingGroups("TestFiles/vic3installation/game/");
	EXPECT_EQ(buildingGroups->getBuildingGroupMap().size(), 6);
}