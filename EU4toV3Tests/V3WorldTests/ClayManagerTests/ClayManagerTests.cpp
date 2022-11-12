#include "ClayManager/ClayManager.h"
#include "gtest/gtest.h"

TEST(V3World_ClayManagerTests, clayManagerCanInitializeVanillaStates)
{
	V3::ClayManager clayManager;

	EXPECT_EQ(0, clayManager.getStates().size());

	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");

	EXPECT_EQ(4, clayManager.getStates().size());
	EXPECT_TRUE(clayManager.getStates().contains("STATE_TEST_1"));
	EXPECT_TRUE(clayManager.getStates().contains("STATE_TEST_2"));
	EXPECT_TRUE(clayManager.getStates().contains("STATE_TEST_3"));
	EXPECT_TRUE(clayManager.getStates().contains("STATE_TEST_4"));
}

TEST(V3World_ClayManagerTests, clayManagerCanLoadTerrainsIntoStateProvinces)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.loadTerrainsIntoProvinces("TestFiles/vic3installation/game/");

	const auto& state2 = clayManager.getStates().at("STATE_TEST_2");
	const auto& province1 = state2->getProvinces().at("xAABBCC");
	const auto& province2 = state2->getProvinces().at("xDDEEFF");

	EXPECT_EQ("lakes", province1->getTerrain());
	EXPECT_EQ("ocean", province2->getTerrain());
}

TEST(V3World_ClayManagerTests, oceanProvincesGetFlaggedAsSeas)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.loadTerrainsIntoProvinces("TestFiles/vic3installation/game/");

	const auto& state2 = clayManager.getStates().at("STATE_TEST_2");
	const auto& province1 = state2->getProvinces().at("xAABBCC");
	const auto& province2 = state2->getProvinces().at("xDDEEFF");

	EXPECT_FALSE(province1->isSea());
	EXPECT_TRUE(province2->isSea());
}

TEST(V3World_ClayManagerTests, lakeProvincesGetFlaggedAsLakesandImpassables)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.loadTerrainsIntoProvinces("TestFiles/vic3installation/game/");

	const auto& state2 = clayManager.getStates().at("STATE_TEST_2");
	const auto& province1 = state2->getProvinces().at("xAABBCC");
	const auto& province2 = state2->getProvinces().at("xDDEEFF");

	EXPECT_TRUE(province1->isLake());
	EXPECT_TRUE(province1->isImpassable());
	EXPECT_FALSE(province2->isLake());
	EXPECT_FALSE(province2->isImpassable());
}


TEST(V3World_ClayManagerTests, clayManagerCanInitializeSuperRegions)
{
	V3::ClayManager clayManager;

	EXPECT_EQ(0, clayManager.getSuperRegions().size());

	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");

	EXPECT_EQ(2, clayManager.getSuperRegions().size());
	EXPECT_TRUE(clayManager.getSuperRegions().contains("test_1_strategic_regions"));
	EXPECT_TRUE(clayManager.getSuperRegions().contains("test_2_strategic_regions"));
}

TEST(V3World_ClayManagerTests, clayManagerCanLinkStatesToSuperRegions)
{
	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates("TestFiles/vic3installation/game/");
	clayManager.initializeSuperRegions("TestFiles/vic3installation/game/");
	clayManager.loadStatesIntoSuperRegions();

	const auto& superRegion1 = clayManager.getSuperRegions().at("test_1_strategic_regions");
	const auto& regionA = superRegion1->getRegions().at("region_a");
	const auto& states = regionA->getStates();

	EXPECT_EQ(2, states.size());

	const auto& state_test_1 = states.at("STATE_TEST_1");

	EXPECT_TRUE(state_test_1);
	EXPECT_EQ(3, state_test_1->getProvinces().size());
	EXPECT_TRUE(state_test_1->getProvinces().at("x445566")->isImpassable());
}
