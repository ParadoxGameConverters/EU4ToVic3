#include "Loaders/StateModifierLoader/StateModifierLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_StateModifierLoaderTests, StateModiferLoaderCanLoadStateModifiers)
{
	V3::StateModifierLoader stateModifierLoader;
	EXPECT_TRUE(stateModifierLoader.getStateModifiers().empty());
	stateModifierLoader.loadStateModifiers("TestFiles/vic3installation/game/");
	EXPECT_EQ(3, stateModifierLoader.getStateModifiers().size());
}

TEST(V3World_StateModifierLoaderTests, StateModiferLoaderSetsStateModifierNames)
{
	V3::StateModifierLoader stateModifierLoader;
	stateModifierLoader.loadStateModifiers("TestFiles/vic3installation/game/");

	const auto stateModifiers = stateModifierLoader.getStateModifiers();

	EXPECT_THAT("state_trait_harbor", stateModifiers.at("state_trait_harbor")->getName());
	EXPECT_THAT("state_trait_falls", stateModifiers.at("state_trait_falls")->getName());
	EXPECT_THAT("state_trait_prairies", stateModifiers.at("state_trait_prairies")->getName());
}