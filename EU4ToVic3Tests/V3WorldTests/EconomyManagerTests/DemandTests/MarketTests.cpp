#include "EconomyManager/Demand/Market.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
#include <ranges>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_MarketTests, DefaultsDefaultToDefaults)
{
	const V3::Market market;

	EXPECT_TRUE(market.getMarketBalance().empty());
}

TEST(V3World_MarketTests, MarketLoadsPossibleGoods)
{
	const V3::Market market({"oil", "paper"});

	EXPECT_THAT(market.getMarketBalance(), testing::UnorderedElementsAre(testing::Pair("oil", 0), testing::Pair("paper", 0)));
}

TEST(V3World_MarketTests, MarketMutatorsMutateMarket)
{
	V3::Market market({"oil"});

	market.sell("oil", 10.0);

	EXPECT_THAT(market.getMarketBalance(), testing::UnorderedElementsAre(testing::Pair("oil", 10)));

	market.buyForBuilding("oil", 5.0);

	EXPECT_THAT(market.getMarketBalance(), testing::UnorderedElementsAre(testing::Pair("oil", 5)));

	market.buyForPop("oil", 10.0);

	EXPECT_THAT(market.getMarketBalance(), testing::UnorderedElementsAre(testing::Pair("oil", -5)));
}

TEST(V3World_MarketTests, MarketShareFollowsFormula)
{
	V3::Market market({"oil", "wood", "fabric", "grain"});
	market.sell("oil", 10);
	market.buyForBuilding("oil", 5);
	market.buyForPop("oil", 10);
	market.sell("wood", 30);
	market.buyForBuilding("wood", 30);
	market.sell("fabric", 10);
	market.sell("grain", 400);

	const auto marketShare = market.getMarketShare(std::vector<std::string>{"oil", "wood", "fabric"});
	EXPECT_THAT(marketShare,
		 testing::UnorderedElementsAre(testing::Pair("oil", 7.5 / 32.5), testing::Pair("wood", 15 / 32.5), testing::Pair("fabric", 10 / 32.5)));
}

TEST(V3World_MarketTests, FormulaPredictsSinglePopSingleNeed)
{
	// Single Pop, Single Need: Poor Food.
	V3::Market market({"grain", "fish", "meat", "groceries", "fruit", "extra"});
	market.sell("grain", 100);
	market.sell("fish", 30);
	market.sell("meat", 40);
	market.sell("groceries", 10);
	market.sell("fruit", 20);

	V3::Vic3DefinesLoader defines;
	defines.loadDefines(modFS);

	V3::DemandLoader demand;
	std::stringstream popNeedsInput;
	std::stringstream buyPackagesInput;
	std::stringstream goodsInput;

	popNeedsInput << "popneed_basic_food = { default = grain\n";
	popNeedsInput << "entry = { goods = grain weight = 1 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = fish weight = 1 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = meat weight = 0.5 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = fruit weight = 0.5 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = groceries weight = 0.75 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	buyPackagesInput << "wealth_7 = { goods = { popneed_basic_food = 118 } }\n";

	goodsInput << "grain = { cost = 20 }\n";
	goodsInput << "fish = { cost = 20 }\n";
	goodsInput << "meat = { cost = 30 }\n";
	goodsInput << "groceries = { cost = 30 }\n";
	goodsInput << "fruit = { cost = 30 }\n";
	goodsInput << "extra = { cost = 90 }\n";

	demand.loadPopNeeds(popNeedsInput);
	demand.loadBuyPackages(buyPackagesInput);
	demand.loadGoods(goodsInput);

	std::stringstream popTypeInput;
	popTypeInput << "strata = poor";
	V3::PopType poors(popTypeInput);
	poors.setType("poors");

	mappers::CultureDef english;
	english.religion = "none";
	english.name = "english";

	mappers::ReligionDef none;
	none.name = "none";

	market.calcPopOrders(53300, {{"poors", 1}}, {{"english", 1}}, defines, demand, {{"poors", poors}}, {{"english", english}}, {{"none", none}}, {}, {});
	EXPECT_THAT(market.getMarketBalance(),
		 testing::UnorderedElementsAre(testing::Pair("extra", testing::DoubleNear(0, 0.01)),
			  testing::Pair("grain", testing::DoubleNear(100 - 11.73, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(30 - 3.52, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(40 - 1.56, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(10 - 0.59, 0.01)),
			  testing::Pair("fruit", testing::DoubleNear(20 - 0.78, 0.01))));
}

// Test Peasants
// Test Aristocrats
// Test Laws (Propertied Woman)
// Test Taboos/Obsessions (various configurations)
// Test Local Goods
// Test 0s
// Test nation-wide
// Test multi-package
// Test building demand