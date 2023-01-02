#include "Loaders/DefinesLoader/EconDefinesLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>


TEST(V3World_EconDefinesLoaderTests, EconDefinesCanBeLoaded)
{
	V3::EconDefinesLoader econDefinesLoader;

	econDefinesLoader.loadEconDefines("TestFiles/configurables/econ_defines.txt");

	EXPECT_EQ(1451750, econDefinesLoader.getGlobalCP());
	EXPECT_DOUBLE_EQ(0.975, econDefinesLoader.getCentralizedPopRatio());
	EXPECT_DOUBLE_EQ(8, econDefinesLoader.getMeanIndustrialScore());
	EXPECT_DOUBLE_EQ(0.5, econDefinesLoader.getStateTraitStrength());
	EXPECT_DOUBLE_EQ(-0.5, econDefinesLoader.getPacketFactor());
}