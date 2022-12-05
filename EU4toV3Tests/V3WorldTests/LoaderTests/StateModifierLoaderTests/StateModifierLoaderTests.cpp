#include "Loaders/StateModifierLoader/StateModifierLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_StateModifierLoaderTests, StateModiferLoaderCanLoadStateModifiers)
{
	V3::StateModifierLoader stateModifierLoader;
	const auto stateModifiers = stateModifierLoader.getStateModifiers();
	EXPECT_TRUE(stateModifiers.empty());
	stateModifierLoader.loadStateModifiers("TestFiles/vic3installation/game/");
	// EXPECT_EQ(buildingGroups->getBuildingGroupMap().size(), 6);
}