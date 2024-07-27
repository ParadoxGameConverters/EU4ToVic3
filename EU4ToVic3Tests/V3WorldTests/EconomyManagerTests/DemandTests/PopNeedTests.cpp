#include "EconomyManager/Demand/PopNeed.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_PopNeedTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;

	const V3::PopNeed popNeed(input);

	EXPECT_EQ("", popNeed.getName());
	EXPECT_EQ("", popNeed.getDefaultGood());
	EXPECT_TRUE(popNeed.getGoodsFulfillment().empty());
}

TEST(V3World_PopNeedTests, PopNeedLoadsGoodsSubstituions)
{
	std::stringstream input;

	input << "default = fabric\n";
	input << "entry = {\n";
	input << "goods = fabric\n";
	input << "weight = 1\n";
	input << "max_supply_share = 0.5\n";
	input << "min_supply_share = 0.0\n";
	input << "}\n";

	input << "entry = {\n";
	input << "goods = clothes\n";
	input << "weight = 2\n";
	input << "max_supply_share = 1.0\n";
	input << "min_supply_share = 0.0\n";
	input << "}\n";

	V3::PopNeed popNeed(input);
	popNeed.setName("popneed_simple_clothing");

	const std::map<std::string, V3::GoodsFulfillment> fulfillment = {{"fabric", {"fabric", 1, 0.5, 0.0}}, {"clothes", {"clothes", 2, 1.0, 0.0}}};

	EXPECT_EQ("popneed_simple_clothing", popNeed.getName());
	EXPECT_EQ("fabric", popNeed.getDefaultGood());
	EXPECT_EQ(fulfillment, popNeed.getGoodsFulfillment());
}