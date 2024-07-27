#include "EconomyManager/Demand/BuyPackage.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_BuyPackageTests, DefaultsDefaultToDefaults)
{
	std::stringstream input;

	const V3::BuyPackage buyPackage(input);

	EXPECT_EQ(0, buyPackage.getWealth());
	EXPECT_TRUE(buyPackage.getPopneeds().empty());
}

TEST(V3World_BuyPackageTests, BuyPackageLoadsPopneeds)
{
	std::stringstream input;

	input << "political_strength = 0.05\n";
	input << "goods = {\n";
	input << "popneed_simple_clothing = 33\n";
	input << "popneed_basic_food = 98\n";
	input << "}\n";

	V3::BuyPackage buyPackage(input);
	buyPackage.setWealth(5);

	const std::map<std::string, int> popneeds = {{"popneed_simple_clothing", 33}, {"popneed_basic_food", 98}};

	EXPECT_EQ(5, buyPackage.getWealth());
	EXPECT_EQ(popneeds, buyPackage.getPopneeds());
}