#include "EconomyManager/Demand/Market.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_MarketTests, DefaultsDefaultToDefaults)
{
	const V3::Market market;

	EXPECT_TRUE(market.getMarketBalance().empty());
}

TEST(V3World_MarketTests, MarketMutatorsMutateMarket)
{
	V3::Market market;

	market.sell("oil", 10.0);

	EXPECT_DOUBLE_EQ(10, market.getMarketBalance().at("oil"));

	market.buyForBuilding("oil", 5.0);

	EXPECT_DOUBLE_EQ(5, market.getMarketBalance().at("oil"));

	market.buyForPop("oil", 10.0);

	EXPECT_DOUBLE_EQ(-5, market.getMarketBalance().at("oil"));
}

TEST(V3World_MarketTests, MarketShareFollowsFormula)
{
	V3::Market market;
	market.sell("oil", 10);
	market.buyForBuilding("oil", 5);
	market.buyForPop("oil", 10);
	market.sell("wood", 30);
	market.buyForBuilding("wood", 30);
	market.sell("fabric", 10);
	market.sell("grain", 400);

	const auto marketShare = market.getMarketShare(std::set<std::string>{"oil", "wood", "fabric"});
	EXPECT_EQ(7.5 / 32.5, marketShare.at("oil"));
	EXPECT_EQ(15 / 32.5, marketShare.at("wood"));
	EXPECT_EQ(10 / 32.5, marketShare.at("fabric"));
	EXPECT_FALSE(marketShare.contains("grain"));
}