#include "EconomyManager/Building/BuildingGroups.h"
#include "Loaders/BuildingLoader/BuildingGroupLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_BuildingGroupsTests, GettersReturnNullOptionals)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	buildingGroupLoader.loadBuildingGroups(modFS);
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();

	const auto parentName = buildingGroups.tryGetParentName("not_a_key0");
	const auto infrastructureCost = buildingGroups.tryGetInfraCost("not_a_key1");

	EXPECT_EQ(std::nullopt, parentName);
	EXPECT_EQ(std::nullopt, infrastructureCost);
}

TEST(V3World_BuildingGroupsTests, BuildingsGroupsSetHierarchyAndCost)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	buildingGroupLoader.loadBuildingGroups(modFS);
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();

	EXPECT_FALSE(buildingGroups.tryGetParentName("bg_manufacturing"));
	EXPECT_FALSE(buildingGroups.tryGetInfraCost("bg_manufacturing"));

	EXPECT_EQ("bg_manufacturing", buildingGroups.tryGetParentName("bg_light_industry").value());
	EXPECT_DOUBLE_EQ(2, buildingGroups.tryGetInfraCost("bg_light_industry").value());

	EXPECT_EQ("bg_mega_industry", buildingGroups.tryGetParentName("bg_giga_industry").value());
	EXPECT_DOUBLE_EQ(1, buildingGroups.tryGetInfraCost("bg_giga_industry").value());
}

TEST(V3World_BuildingGroupsTests, TopLevelAncestorIsDiscoverable)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	buildingGroupLoader.loadBuildingGroups(modFS);
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();

	EXPECT_EQ("bg_manufacturing", buildingGroups.getAncestralGroup("bg_giga_industry").value());
}

TEST(V3World_BuildingGroupsTests, TopLevelAncestorReturnsSelf)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	buildingGroupLoader.loadBuildingGroups(modFS);
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();

	EXPECT_EQ("bg_manufacturing", buildingGroups.getAncestralGroup("bg_manufacturing").value());
}

TEST(V3World_BuildingGroupsTests, InvalidGroupReturnsNullAncestor)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	buildingGroupLoader.loadBuildingGroups(modFS);
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();

	EXPECT_EQ(std::nullopt, buildingGroups.getAncestralGroup("bg_none"));
}

TEST(V3World_BuildingGroupsTests, UrbanizationLooksThroughHeirarchy)
{
	V3::BuildingGroupLoader buildingGroupLoader;
	buildingGroupLoader.loadBuildingGroups(modFS);
	const auto buildingGroups = buildingGroupLoader.getBuildingGroups();

	EXPECT_EQ(0, buildingGroups.getUrbanization("bg_manufacturing"));
	EXPECT_EQ(20, buildingGroups.getUrbanization("bg_light_industry"));
	EXPECT_EQ(30, buildingGroups.getUrbanization("bg_giga_industry"));
}