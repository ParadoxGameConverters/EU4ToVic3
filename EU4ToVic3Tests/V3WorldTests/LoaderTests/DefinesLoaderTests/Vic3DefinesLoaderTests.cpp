#include "Loaders/DefinesLoader/Vic3DefinesLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game", {});
}

TEST(V3World_Vic3DefinesLoaderTests, GameDefinesAreLoaded)
{
	V3::Vic3DefinesLoader vic3DefinesLoader;
	vic3DefinesLoader.loadDefines(modFS);

	EXPECT_DOUBLE_EQ(5.0, vic3DefinesLoader.getSplitStatePrimeLandWeight());
	EXPECT_EQ(100000, vic3DefinesLoader.getIndividualsPerPopInfrastructure());
	EXPECT_EQ(10, vic3DefinesLoader.getStateBureaucracyBaseCost());
	EXPECT_DOUBLE_EQ(4.0, vic3DefinesLoader.getStateBureaucracyPopBaseCost());
	EXPECT_DOUBLE_EQ(100000.0, vic3DefinesLoader.getStateBureaucracyPopMultiple());
	EXPECT_DOUBLE_EQ(10, vic3DefinesLoader.getMinimumInvestmentCost());
	EXPECT_DOUBLE_EQ(0.25, vic3DefinesLoader.getWorkingAdultRatioBase());
	EXPECT_DOUBLE_EQ(0.5, vic3DefinesLoader.getDependentConsumptionRatio());
}