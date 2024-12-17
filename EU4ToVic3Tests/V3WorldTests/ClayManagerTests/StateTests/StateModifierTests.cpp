#include "ClayManager/State/StateModifier.h"
#include "Loaders/BuildingLoader/BuildingGroupLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_StateModifierTests, DefaultsDefaultToDefaults)
{
	V3::StateModifier modifier;

	EXPECT_TRUE(modifier.getName().empty());
	EXPECT_EQ(0, modifier.getInfrastructureBonus());
	EXPECT_EQ(0, modifier.getInfrastructureMult());
	EXPECT_FALSE(modifier.getMaxBuildingBonus("building_port"));
	EXPECT_FALSE(modifier.getMaxBuildingBonus("building_naval_base"));
	EXPECT_TRUE(modifier.getBuildingGroupModifiersMap().empty());
	EXPECT_TRUE(modifier.getBuildingModifiersMap().empty());
	EXPECT_TRUE(modifier.getGoodsModifiersMap().empty());
}

TEST(V3World_StateModifierTests, InfrastructureBonusIsSet)
{
	std::stringstream input;
	input << "\ticon = \"gfx/ignore/me.dds\"\n";
	input << "\tmodifier = {\n";
	input << "\t\tstate_infrastructure_add = 10\n";
	input << "\t}\n";

	V3::StateModifier modifier;
	modifier.loadStateModifier(input);

	EXPECT_EQ(10, modifier.getInfrastructureBonus());
}

TEST(V3World_StateModifierTests, InfrastructureModifierIsSet)
{
	std::stringstream input;
	input << "\ticon = \"gfx/ignore/me.dds\"\n";
	input << "\tmodifier = {\n";
	input << "\t\tstate_infrastructure_mult = -0.4\n";
	input << "\t}\n";

	V3::StateModifier modifier;
	modifier.loadStateModifier(input);

	EXPECT_EQ(-0.4, modifier.getInfrastructureMult());
}

TEST(V3World_StateModifierTests, MaxBonusesAreSet)
{
	std::stringstream input;
	input << "\ticon = \"gfx/ignore/me.dds\"\n";
	input << "\tmodifier = {\n";
	input << "\t\tstate_building_port_max_level_add = 2\n";
	input << "\t\tstate_building_naval_base_max_level_add = 20\n";
	input << "\t}\n";

	V3::StateModifier modifier;
	modifier.loadStateModifier(input);

	EXPECT_EQ(2, modifier.getMaxBuildingBonus("building_port"));
	EXPECT_EQ(20, modifier.getMaxBuildingBonus("building_naval_base"));
}

TEST(V3World_StateModifierTests, BuildingGroupModifiersAreSet)
{
	std::stringstream input;
	input << "\ticon = \"gfx/ignore/me.dds\"\n";
	input << "\tmodifier = {\n";
	input << "\t\tbuilding_group_bg_agri_throughput_mult = 0.2\n";
	input << "\t\tbuilding_group_bg_fish_throughput_mult = 0.3\n";
	input << "\t}\n";

	V3::StateModifier modifier;
	modifier.loadStateModifier(input);

	EXPECT_DOUBLE_EQ(0.2, modifier.getBuildingGroupModifier("bg_agri", V3::BuildingGroups()));
	EXPECT_DOUBLE_EQ(0.3, modifier.getBuildingGroupModifier("bg_fish", V3::BuildingGroups()));
}

TEST(V3World_StateModifierTests, BuildingModifiersAreSet)
{
	std::stringstream input;
	input << "\ticon = \"gfx/ignore/me.dds\"\n";
	input << "\tmodifier = {\n";
	input << "\t\tbuilding_house_throughput_mult = 0.2\n";
	input << "\t\tbuilding_factory_throughput_mult = 0.3\n";
	input << "\t}\n";

	V3::StateModifier modifier;
	modifier.loadStateModifier(input);

	EXPECT_DOUBLE_EQ(0.2, modifier.getBuildingModifier("building_house").value());
	EXPECT_DOUBLE_EQ(0.3, modifier.getBuildingModifier("building_factory").value());
}


TEST(V3World_StateModifierTests, GoodsModifiersAreSet)
{
	std::stringstream input;
	input << "\ticon = \"gfx/ignore/me.dds\"\n";
	input << "\tmodifier = {\n";
	input << "\t\tbuilding_output_lime_mult = 0.2\n";
	input << "\t\tbuilding_output_coconut_mult = 0.3\n";
	input << "\t}\n";

	V3::StateModifier modifier;
	modifier.loadStateModifier(input);

	EXPECT_DOUBLE_EQ(0.2, modifier.getGoodsModifier("building_output_lime").value());
	EXPECT_DOUBLE_EQ(0.3, modifier.getGoodsModifier("building_output_coconut").value());
}

TEST(V3World_StateModifierTests, RegexDontCollide)
{
	std::stringstream input;
	input << "\ticon = \"gfx/ignore/me.dds\"\n";
	input << "\tmodifier = {\n";
	input << "\t\tbuilding_group_bg_fish_throughput_mult = 0.4\n";
	input << "\t\tbuilding_output_coconut_mult = 0.3\n";
	input << "\t\tbuilding_house_throughput_mult = 0.2\n";
	input << "\t}\n";

	V3::StateModifier modifier;
	modifier.loadStateModifier(input);

	EXPECT_DOUBLE_EQ(0.2, modifier.getBuildingModifier("building_house").value());
	EXPECT_DOUBLE_EQ(0.3, modifier.getGoodsModifier("building_output_coconut").value());
	EXPECT_DOUBLE_EQ(0.4, modifier.getBuildingGroupModifier("bg_fish", V3::BuildingGroups()));
}

TEST(V3World_StateModifierTests, GettingBuildingGroupDataTravelsHeirarchy)
{
	std::stringstream input;
	input << "\ticon = \"gfx/ignore/me.dds\"\n";
	input << "\tmodifier = {\n";
	input << "\t\tbuilding_group_bg_manufacturing_throughput_mult = 0.2\n";
	input << "\t}\n";

	V3::StateModifier modifier;
	modifier.loadStateModifier(input);

	V3::BuildingGroupLoader buildingGroupLoader;
	buildingGroupLoader.loadBuildingGroups(modFS);
	auto buildingGroups = buildingGroupLoader.getBuildingGroups();

	EXPECT_DOUBLE_EQ(0.2, modifier.getBuildingGroupModifier("bg_light_industry", buildingGroups));
}

TEST(V3World_StateModifierTests, GetAllBonusesCombinesLikeBonuses)
{
	std::stringstream input;
	input << "\ticon = \"gfx/ignore/me.dds\"\n";
	input << "\tmodifier = {\n";
	input << "\t\tbuilding_group_bg_manufacturing_throughput_mult = 0.2\n";
	input << "\t\tbuilding_group_bg_fish_throughput_mult = 0.4\n";
	input << "\t\tbuilding_group_bg_coconut_throughput_mult = -0.1\n";
	input << "\t\tbuilding_output_lime_mult = 0.1\n";
	input << "\t\tbuilding_output_coconut_mult = 0.2\n";
	input << "\t}\n";

	V3::StateModifier modifier;
	modifier.loadStateModifier(input);

	EXPECT_DOUBLE_EQ(0.5, V3::StateModifier::getAllBonuses(modifier.getBuildingGroupModifiersMap()));
	EXPECT_DOUBLE_EQ(0, V3::StateModifier::getAllBonuses(modifier.getBuildingModifiersMap()));
	EXPECT_DOUBLE_EQ(0.3, V3::StateModifier::getAllBonuses(modifier.getGoodsModifiersMap()));
}

TEST(V3World_StateModifierTests, GivenBuildingAllModifiersAreSummed)
{
	std::stringstream buildingInput;
	buildingInput << "building_group = bg_light_industry\n";

	V3::Building building;
	building.loadBuilding(buildingInput, {});
	building.setName("building_factory");

	std::stringstream modInput;
	modInput << "\ticon = \"gfx/ignore/me.dds\"\n";
	modInput << "\tmodifier = {\n";
	modInput << "\t\tbuilding_house_throughput_mult = 0.2\n";
	modInput << "\t\tbuilding_factory_throughput_mult = 0.3\n";
	modInput << "\t\tbuilding_group_bg_manufacturing_throughput_mult = 0.4\n";
	modInput << "\t\tbuilding_group_bg_light_industry_throughput_mult = 0.5\n";
	modInput << "\t}\n";

	V3::BuildingGroupLoader buildingGroupLoader;
	buildingGroupLoader.loadBuildingGroups(modFS);
	auto buildingGroups = buildingGroupLoader.getBuildingGroups();

	V3::StateModifier modifier;
	modifier.loadStateModifier(modInput);

	EXPECT_DOUBLE_EQ(1.2, modifier.calcBuildingModifiers(building, buildingGroups));
}