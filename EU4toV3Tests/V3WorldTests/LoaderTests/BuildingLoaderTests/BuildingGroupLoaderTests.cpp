#include "EconomyManager/Building/BuildingGroups.h"
#include "Loaders/BuildingLoader/BuildingGroupLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderCanLoadBuildingGroups)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	EXPECT_TRUE(buildingGroupLoader.getBuildingGroups().getBuildingGroupMap().empty());
	buildingGroupLoader.loadBuildingGroups(modFS);
	EXPECT_EQ(6, buildingGroupLoader.getBuildingGroups().getBuildingGroupMap().size());
}

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderTracksHierarchy)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	buildingGroupLoader.loadBuildingGroups(modFS);
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();

	EXPECT_TRUE(buildingGroups.getParentName("bg_manufacturing")->empty());
	EXPECT_EQ("bg_manufacturing", buildingGroups.getParentName("bg_light_industry").value());
	EXPECT_EQ("bg_manufacturing", buildingGroups.getParentName("bg_heavy_industry").value());
	EXPECT_EQ("bg_heavy_industry", buildingGroups.getParentName("bg_ultra_industry").value());
	EXPECT_EQ("bg_ultra_industry", buildingGroups.getParentName("bg_mega_industry").value());
	EXPECT_EQ("bg_mega_industry", buildingGroups.getParentName("bg_giga_industry").value());
}

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderSetsInfrastructureInheritance)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	buildingGroupLoader.loadBuildingGroups(modFS);
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();

	EXPECT_EQ(0, buildingGroups.getInfrastructureCost("bg_manufacturing").value());
	EXPECT_EQ(2, buildingGroups.getInfrastructureCost("bg_light_industry").value());
	EXPECT_EQ(3, buildingGroups.getInfrastructureCost("bg_heavy_industry").value());
	EXPECT_EQ(1, buildingGroups.getInfrastructureCost("bg_ultra_industry").value());
	EXPECT_EQ(1, buildingGroups.getInfrastructureCost("bg_mega_industry").value());
	EXPECT_EQ(1, buildingGroups.getInfrastructureCost("bg_giga_industry").value());
}
