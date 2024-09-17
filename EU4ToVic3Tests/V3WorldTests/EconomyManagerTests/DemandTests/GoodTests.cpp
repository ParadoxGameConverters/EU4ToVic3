#include "EconomyManager/Demand/Good.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_GoodTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;

	const V3::Good good(input);

	EXPECT_EQ("", good.getName());
	EXPECT_EQ(0, good.getBasePrice());
	EXPECT_FALSE(good.isLocal());
}

TEST(V3World_GoodTests, GoodLoadsGoodData)
{
	std::stringstream input;

	input << "texture = \"gfx/interface/icons/goods_icons/transportation.dds\"\n";
	input << "cost = 30\n";
	input << "category = staple\n";
	input << "local = yes\n";
	input << "\n";
	input << "prestige_factor = 4\n";
	input << "\n";
	input << "consumption_tax_cost = 200\n";

	V3::Good good(input);
	good.setName("transportation");

	EXPECT_EQ("transportation", good.getName());
	EXPECT_EQ(30, good.getBasePrice());
	EXPECT_TRUE(good.isLocal());
}