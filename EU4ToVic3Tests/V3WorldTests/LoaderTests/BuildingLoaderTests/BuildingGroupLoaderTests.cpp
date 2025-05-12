#include "EconomyManager/Building/BuildingGroups.h"
#include "Loaders/BuildingLoader/BuildingGroupLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

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

	EXPECT_FALSE(buildingGroups.tryGetParentName("bg_manufacturing"));
	EXPECT_EQ("bg_manufacturing", buildingGroups.tryGetParentName("bg_light_industry").value());
	EXPECT_EQ("bg_manufacturing", buildingGroups.tryGetParentName("bg_heavy_industry").value());
	EXPECT_EQ("bg_heavy_industry", buildingGroups.tryGetParentName("bg_ultra_industry").value());
	EXPECT_EQ("bg_ultra_industry", buildingGroups.tryGetParentName("bg_mega_industry").value());
	EXPECT_EQ("bg_mega_industry", buildingGroups.tryGetParentName("bg_giga_industry").value());
}

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderSetsInfrastructureInheritance)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	buildingGroupLoader.loadBuildingGroups(modFS);
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();

	EXPECT_FALSE(buildingGroups.tryGetInfraCost("bg_manufacturing"));
	EXPECT_DOUBLE_EQ(2, buildingGroups.tryGetInfraCost("bg_light_industry").value());
	EXPECT_DOUBLE_EQ(3, buildingGroups.tryGetInfraCost("bg_heavy_industry").value());
	EXPECT_DOUBLE_EQ(1, buildingGroups.tryGetInfraCost("bg_ultra_industry").value());
	EXPECT_DOUBLE_EQ(1, buildingGroups.tryGetInfraCost("bg_mega_industry").value());
	EXPECT_DOUBLE_EQ(1, buildingGroups.tryGetInfraCost("bg_giga_industry").value());
}

TEST(V3World_BuildingGroupLoaderTests, BuildingGroupLoaderSetsCappedResourcesFromInheritance)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	buildingGroupLoader.loadBuildingGroups(modFS);
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();

	EXPECT_TRUE(buildingGroups.tryGetIsCapped("bg_manufacturing").value());
	EXPECT_TRUE(buildingGroups.tryGetIsCapped("bg_light_industry").value());
	EXPECT_FALSE(buildingGroups.tryGetIsCapped("bg_heavy_industry").value());
	EXPECT_FALSE(buildingGroups.tryGetIsCapped("bg_ultra_industry").value());
	EXPECT_TRUE(buildingGroups.tryGetIsCapped("bg_mega_industry").value());
	EXPECT_TRUE(buildingGroups.tryGetIsCapped("bg_giga_industry").value());
}
