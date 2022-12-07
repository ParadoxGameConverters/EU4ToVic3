#include "ClayManager/State/StateModifier.h"
#include "Loaders/BuildingLoader/BuildingGroupLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
TEST(V3World_StateModifierTests, DefaultsDefaultToDefaults)
{
	V3::StateModifier modifier;

	EXPECT_TRUE(modifier.getName().empty());
	EXPECT_EQ(0, modifier.getInfrastructureBonus());
	EXPECT_EQ(0, modifier.getInfrastructureModifier());
	EXPECT_EQ(0, modifier.getPortBonus());
	EXPECT_EQ(0, modifier.getNavalBaseBonus());
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

	EXPECT_EQ(-0.4, modifier.getInfrastructureModifier());
}

TEST(V3World_StateModifierTests, PortBonusIsSet)
{
	std::stringstream input;
	input << "\ticon = \"gfx/ignore/me.dds\"\n";
	input << "\tmodifier = {\n";
	input << "\t\tstate_building_port_max_level_add = 2\n";
	input << "\t}\n";

	V3::StateModifier modifier;
	modifier.loadStateModifier(input);

	EXPECT_EQ(2, modifier.getPortBonus());
}

TEST(V3World_StateModifierTests, NavalBaseBonusIsSet)
{
	std::stringstream input;
	input << "\ticon = \"gfx/ignore/me.dds\"\n";
	input << "\tmodifier = {\n";
	input << "\t\tstate_building_naval_base_max_level_add = 5\n";
	input << "\t}\n";

	V3::StateModifier modifier;
	modifier.loadStateModifier(input);

	EXPECT_EQ(5, modifier.getNavalBaseBonus());
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

	EXPECT_DOUBLE_EQ(0.2, modifier.getBuildingGroupModifier("bg_agri", std::shared_ptr<V3::BuildingGroups>()).value());
	EXPECT_DOUBLE_EQ(0.3, modifier.getBuildingGroupModifier("bg_fish", std::shared_ptr<V3::BuildingGroups>()).value());
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
	EXPECT_DOUBLE_EQ(0.4, modifier.getBuildingGroupModifier("bg_fish", std::shared_ptr<V3::BuildingGroups>()).value());
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

	EXPECT_DOUBLE_EQ(0.2, modifier.getBuildingGroupModifier("bg_light_industry", buildingGroups).value());
}