#include "ClayManager/State/State.h"
#include "Loaders/StateLoader/StateLoader.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_StateLoaderTests, stateLoaderCanLoadStates)
{
	V3::StateLoader stateLoader;

	EXPECT_EQ(0, stateLoader.getStates().size());

	stateLoader.loadStates(modFS);

	EXPECT_EQ(10, stateLoader.getStates().size());

	const auto& state1 = stateLoader.getStates().at("STATE_TEST_1");
	const auto& state2 = stateLoader.getStates().at("STATE_TEST_2");
	const auto& state3 = stateLoader.getStates().at("STATE_TEST_3");
	const auto& state4 = stateLoader.getStates().at("STATE_TEST_4");
	const auto& state5 = stateLoader.getStates().at("STATE_TEST_LAKE");

	EXPECT_TRUE(state1->isCoastal());
	EXPECT_EQ("STATE_TEST_1", state1->getName());
	EXPECT_TRUE(state1->containsProvince("x112233"));
	EXPECT_TRUE(state1->containsProvince("x445566"));
	EXPECT_TRUE(state1->containsProvince("x778899"));

	EXPECT_FALSE(state2->isCoastal());
	EXPECT_EQ("STATE_TEST_2", state2->getName());
	EXPECT_TRUE(state2->containsProvince("xDDEEFF"));

	EXPECT_TRUE(state3->isCoastal());
	EXPECT_EQ("STATE_TEST_3", state3->getName());
	EXPECT_TRUE(state3->containsProvince("x123456"));
	EXPECT_TRUE(state3->containsProvince("x7890AB"));
	EXPECT_TRUE(state3->containsProvince("xCDEF12"));

	EXPECT_FALSE(state4->isCoastal());
	EXPECT_EQ("STATE_TEST_4", state4->getName());
	EXPECT_TRUE(state4->containsProvince("x345678"));
	EXPECT_TRUE(state4->containsProvince("x9ABCDE"));

	EXPECT_FALSE(state5->isCoastal());
	EXPECT_EQ("STATE_TEST_LAKE", state5->getName());
	EXPECT_TRUE(state5->containsProvince("xAABBCC"));
}

TEST(V3World_StateLoaderTests, nonTXTfilesAreIgnored)
{
	V3::StateLoader stateLoader;

	EXPECT_EQ(0, stateLoader.getStates().size());

	stateLoader.loadStates(modFS);

	EXPECT_EQ(10, stateLoader.getStates().size());

	EXPECT_FALSE(stateLoader.getStates().contains("STATE_TEST_5"));
}
