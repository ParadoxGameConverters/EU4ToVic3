#include "Loaders/DefinesLoader/EconDefinesLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>


TEST(V3World_EconDefinesLoaderTests, EconDefinesCanBeLoaded)
{
	V3::EconDefinesLoader econDefinesLoader;

	EXPECT_EQ(0, econDefinesLoader.getGlobalCP());
	EXPECT_DOUBLE_EQ(0, econDefinesLoader.getCentralizedPopRatio());
	EXPECT_DOUBLE_EQ(0, econDefinesLoader.getMeanCivlevel());
	EXPECT_DOUBLE_EQ(0, econDefinesLoader.getStateTraitStrength());
	EXPECT_DOUBLE_EQ(0, econDefinesLoader.getPacketFactor());
	EXPECT_DOUBLE_EQ(0, econDefinesLoader.getIncorporatedModifier());
	EXPECT_DOUBLE_EQ(0, econDefinesLoader.getMinDevPerPop());
	EXPECT_DOUBLE_EQ(1, econDefinesLoader.getMaxDevPerPop());

	econDefinesLoader.loadEconDefines("TestFiles/configurables/economy/econ_defines.txt");

	EXPECT_EQ(1451750, econDefinesLoader.getGlobalCP());
	EXPECT_DOUBLE_EQ(0.975, econDefinesLoader.getCentralizedPopRatio());
	EXPECT_DOUBLE_EQ(80, econDefinesLoader.getMeanCivlevel());
	EXPECT_DOUBLE_EQ(0.5, econDefinesLoader.getStateTraitStrength());
	EXPECT_DOUBLE_EQ(-0.5, econDefinesLoader.getPacketFactor());
	EXPECT_DOUBLE_EQ(-0.3, econDefinesLoader.getIncorporatedModifier());
	EXPECT_DOUBLE_EQ(0.0000016, econDefinesLoader.getMinDevPerPop());
	EXPECT_DOUBLE_EQ(0.002, econDefinesLoader.getMaxDevPerPop());
}