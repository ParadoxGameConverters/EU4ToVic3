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

TEST(V3World_MarketTests, ClearMarketResetsMarket)
{
	V3::Market market({"oil"});

	market.sell("oil", 10.0);
	market.buyForBuilding("oil", 5.0);
	market.buyForPop("oil", 10.0);

	EXPECT_THAT(market.getMarketBalance(), testing::UnorderedElementsAre(testing::Pair("oil", -5)));

	market.clearMarket();

	EXPECT_THAT(market.getMarketBalance(), testing::UnorderedElementsAre(testing::Pair("oil", 0)));
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

	buyPackagesInput << "wealth_8 = { goods = { popneed_basic_food = 118 } }\n";

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

TEST(V3World_MarketTests, FormulaBuildingEffectOnNeed)
{
	// Building buy orders discount supply by 1/2.
	V3::Market market({"grain", "fish", "meat", "groceries", "fruit", "extra"});
	market.sell("grain", 100);
	market.sell("fish", 30);
	market.sell("meat", 40);
	market.sell("groceries", 10);
	market.sell("fruit", 20);
	market.buyForBuilding("grain", 90);
	market.buyForBuilding("meat", 40);

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

	buyPackagesInput << "wealth_8 = { goods = { popneed_basic_food = 118 } }\n";

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
			  testing::Pair("grain", testing::DoubleNear(10 - 9.61, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(30 - 5.24, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(-1.16, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(10 - 0.87, 0.01)),
			  testing::Pair("fruit", testing::DoubleNear(20 - 1.16, 0.01))));
}

TEST(V3World_MarketTests, FormulaPredictsPeasantsNeed)
{
	// A pop with a non-default consumption_mult changes the size of the need.
	// (We incorporated this into the popFactor instead of the buy package size).
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

	buyPackagesInput << "wealth_8 = { goods = { popneed_basic_food = 118 } }\n";

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
	popTypeInput << "strata = poor\n";
	popTypeInput << "consumption_mult = 0.05\n";
	V3::PopType peasants(popTypeInput);
	peasants.setType("peasants");

	mappers::CultureDef english;
	english.religion = "none";
	english.name = "english";

	mappers::ReligionDef none;
	none.name = "none";

	market
		 .calcPopOrders(53300, {{"peasants", 1}}, {{"english", 1}}, defines, demand, {{"peasants", peasants}}, {{"english", english}}, {{"none", none}}, {}, {});
	EXPECT_THAT(market.getMarketBalance(),
		 testing::UnorderedElementsAre(testing::Pair("extra", testing::DoubleNear(0, 0.01)),
			  testing::Pair("grain", testing::DoubleNear(100 - 0.59, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(30 - 0.18, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(40 - 0.08, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(10 - 0.03, 0.01)),
			  testing::Pair("fruit", testing::DoubleNear(20 - 0.04, 0.01))));
}

TEST(V3World_MarketTests, FormulaPredictsAristocratsNeed)
{
	// A pop with a non-default working_adult_ratio changes the popFactor.
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

	buyPackagesInput << "wealth_30 = { goods = { popneed_basic_food = 118 } }\n";

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
	popTypeInput << "start_quality_of_life = 20\n";
	popTypeInput << "working_adult_ratio = 0.2\n";
	V3::PopType nobles(popTypeInput);
	nobles.setType("nobles");

	mappers::CultureDef english;
	english.religion = "none";
	english.name = "english";

	mappers::ReligionDef none;
	none.name = "none";

	market.calcPopOrders(53300, {{"nobles", 1}}, {{"english", 1}}, defines, demand, {{"nobles", nobles}}, {{"english", english}}, {{"none", none}}, {}, {});
	EXPECT_THAT(market.getMarketBalance(),
		 testing::UnorderedElementsAre(testing::Pair("extra", testing::DoubleNear(0, 0.01)),
			  testing::Pair("grain", testing::DoubleNear(100 - 11.26, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(30 - 3.38, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(40 - 1.50, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(10 - 0.56, 0.01)),
			  testing::Pair("fruit", testing::DoubleNear(20 - 0.75, 0.01))));
}

TEST(V3World_MarketTests, FormulaPredictsObsessionsNeed)
{
	// Cultural Obsession increases minimum weight, obsessed goods, and overall need.
	V3::Market market({"grain", "fish", "meat", "groceries", "wine", "extra"});
	market.sell("grain", 100);
	market.sell("fish", 30);
	market.sell("meat", 40);
	market.sell("groceries", 10);
	market.sell("wine", 20);

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
	popNeedsInput << "entry = { goods = wine weight = 0.5 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = groceries weight = 0.75 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	buyPackagesInput << "wealth_8 = { goods = { popneed_basic_food = 118 } }\n";

	goodsInput << "grain = { cost = 20 }\n";
	goodsInput << "fish = { cost = 20 }\n";
	goodsInput << "meat = { cost = 30 }\n";
	goodsInput << "groceries = { cost = 30 }\n";
	goodsInput << "wine = { cost = 30 }\n";
	goodsInput << "extra = { cost = 90 }\n";

	demand.loadPopNeeds(popNeedsInput);
	demand.loadBuyPackages(buyPackagesInput);
	demand.loadGoods(goodsInput);

	std::stringstream popTypeInput;
	popTypeInput << "strata = poor\n";
	V3::PopType poors(popTypeInput);
	poors.setType("poors");

	mappers::CultureDef french;
	french.religion = "none";
	french.name = "french";
	french.obsessions.insert("wine");

	mappers::ReligionDef none;
	none.name = "none";

	market.calcPopOrders(53300, {{"poors", 1}}, {{"french", 1}}, defines, demand, {{"poors", poors}}, {{"french", french}}, {{"none", none}}, {}, {});
	EXPECT_THAT(market.getMarketBalance(),
		 testing::UnorderedElementsAre(testing::Pair("extra", testing::DoubleNear(0, 0.01)),
			  testing::Pair("grain", testing::DoubleNear(100 - 12.44, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(30 - 3.73, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(40 - 1.66, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(10 - 0.62, 0.01)),
			  testing::Pair("wine", testing::DoubleNear(20 - 3.32, 0.01))));
}

TEST(V3World_MarketTests, FormulaPredictsTaboosNeed)
{
	// Religious Taboo suppresses both the tabooed goods and the overall need.
	// Effect caps at +-25%
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

	buyPackagesInput << "wealth_8 = { goods = { popneed_basic_food = 118 } }\n";

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
	english.religion = "eatmeat";
	english.name = "english";

	mappers::ReligionDef eatmeat;
	eatmeat.name = "eatmeat";
	eatmeat.taboos.insert("grain");
	eatmeat.taboos.insert("fish");

	market.calcPopOrders(53300, {{"poors", 1}}, {{"english", 1}}, defines, demand, {{"poors", poors}}, {{"english", english}}, {{"eatmeat", eatmeat}}, {}, {});
	EXPECT_THAT(market.getMarketBalance(),
		 testing::UnorderedElementsAre(testing::Pair("extra", testing::DoubleNear(0, 0.01)),
			  testing::Pair("grain", testing::DoubleNear(100 - 7.19, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(30 - 2.16, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(40 - 1.92, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(10 - 0.72, 0.01)),
			  testing::Pair("fruit", testing::DoubleNear(20 - 0.96, 0.01))));
}

TEST(V3World_MarketTests, FormulaPredictsObsessionsAndTaboosNeed)
{
	// The French Obsession for Wine and the Sunni Taboo on Wine cancel each other out.
	V3::Market market({"grain", "fish", "meat", "groceries", "wine", "extra"});
	market.sell("grain", 100);
	market.sell("fish", 30);
	market.sell("meat", 40);
	market.sell("groceries", 10);
	market.sell("wine", 20);

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
	popNeedsInput << "entry = { goods = wine weight = 0.5 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = groceries weight = 0.75 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	buyPackagesInput << "wealth_8 = { goods = { popneed_basic_food = 118 } }\n";

	goodsInput << "grain = { cost = 20 }\n";
	goodsInput << "fish = { cost = 20 }\n";
	goodsInput << "meat = { cost = 30 }\n";
	goodsInput << "groceries = { cost = 30 }\n";
	goodsInput << "wine = { cost = 30 }\n";
	goodsInput << "extra = { cost = 90 }\n";

	demand.loadPopNeeds(popNeedsInput);
	demand.loadBuyPackages(buyPackagesInput);
	demand.loadGoods(goodsInput);

	std::stringstream popTypeInput;
	popTypeInput << "strata = poor";
	V3::PopType poors(popTypeInput);
	poors.setType("poors");

	mappers::CultureDef french;
	french.religion = "sunni";
	french.name = "french";
	french.obsessions.insert("wine");

	mappers::ReligionDef sunni;
	sunni.name = "sunni";
	sunni.taboos.insert("wine");

	market.calcPopOrders(53300, {{"poors", 1}}, {{"french", 1}}, defines, demand, {{"poors", poors}}, {{"french", french}}, {{"sunni", sunni}}, {}, {});
	EXPECT_THAT(market.getMarketBalance(),
		 testing::UnorderedElementsAre(testing::Pair("extra", testing::DoubleNear(0, 0.01)),
			  testing::Pair("grain", testing::DoubleNear(100 - 11.73, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(30 - 3.52, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(40 - 1.56, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(10 - 0.59, 0.01)),
			  testing::Pair("wine", testing::DoubleNear(20 - 0.78, 0.01))));
}

TEST(V3World_MarketTests, FormulaPredictsEmpoweredWomenNeed)
{
	// Country passes in a law with the working_adult_ratio_add modifier.
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

	buyPackagesInput << "wealth_8 = { goods = { popneed_basic_food = 118 } }\n";

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

	V3::Law own_property, emptyLaw;
	own_property.workingAdultRatioAdd = 0.05;

	market.calcPopOrders(53300,
		 {{"poors", 1}},
		 {{"english", 1}},
		 defines,
		 demand,
		 {{"poors", poors}},
		 {{"english", english}},
		 {{"none", none}},
		 {"law_women_own_property"},
		 {{"law_women_own_property", own_property}, {"", emptyLaw}});

	EXPECT_THAT(market.getMarketBalance(),
		 testing::UnorderedElementsAre(testing::Pair("extra", testing::DoubleNear(0, 0.01)),
			  testing::Pair("grain", testing::DoubleNear(100 - 12.2, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(30 - 3.66, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(40 - 1.63, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(10 - 0.61, 0.01)),
			  testing::Pair("fruit", testing::DoubleNear(20 - 0.81, 0.01))));
}

TEST(V3World_MarketTests, FormulaAssumesMinimumSupply)
{
	// All supply is either 0 or negative.
	// Should avoid / 0 errors.
	// Predicted values should be positive, non-zero.
	V3::Market market({"grain", "fish", "meat", "groceries", "fruit", "extra"});
	market.buyForBuilding("fish", 20);
	market.buyForBuilding("fruit", 20);

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

	buyPackagesInput << "wealth_8 = { goods = { popneed_basic_food = 118 } }\n";

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
			  testing::Pair("grain", testing::DoubleNear(-5.24, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(-20 - 5.24, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(-1.75, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(-2.62, 0.01)),
			  testing::Pair("fruit", testing::DoubleNear(-20 - 1.75, 0.01))));
}

TEST(V3World_MarketTests, MissingGoodGivesWarning)
{
	// Market does not load fruit. Expect to raise a Log(LogLevel::Error) about this. Don't crash.
	V3::Market market({"grain", "fish", "meat", "groceries", "extra"});
	market.sell("grain", 100);
	market.sell("fish", 30);
	market.sell("meat", 40);
	market.sell("groceries", 10);

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	market.sell("fruit", 20);

	std::cout.rdbuf(cout_buffer);

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

	buyPackagesInput << "wealth_8 = { goods = { popneed_basic_food = 118 } }\n";

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

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Good: fruit not recognized in market. Converter will act like it doesn't exist.)"));
	EXPECT_THAT(market.getMarketBalance(),
		 testing::UnorderedElementsAre(testing::Pair("extra", testing::DoubleNear(0, 0.01)),
			  testing::Pair("grain", testing::DoubleNear(100 - 12.48, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(30 - 3.74, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(40 - 1.66, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(10 - 0.62, 0.01))));
}

TEST(V3World_MarketTests, MissingCultureGivesWarning)
{
	// Input Culture lacks a CultureDef mapping. Expect to raise a Log(LogLevel::Warning) about this. Don't crash.
	V3::Market market({"grain", "fish", "meat", "groceries", "wine", "extra"});
	market.sell("grain", 100);
	market.sell("fish", 30);
	market.sell("meat", 40);
	market.sell("groceries", 10);
	market.sell("wine", 20);

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
	popNeedsInput << "entry = { goods = wine weight = 0.5 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = groceries weight = 0.75 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	buyPackagesInput << "wealth_8 = { goods = { popneed_basic_food = 118 } }\n";

	goodsInput << "grain = { cost = 20 }\n";
	goodsInput << "fish = { cost = 20 }\n";
	goodsInput << "meat = { cost = 30 }\n";
	goodsInput << "groceries = { cost = 30 }\n";
	goodsInput << "wine = { cost = 30 }\n";
	goodsInput << "extra = { cost = 90 }\n";

	demand.loadPopNeeds(popNeedsInput);
	demand.loadBuyPackages(buyPackagesInput);
	demand.loadGoods(goodsInput);

	std::stringstream popTypeInput;
	popTypeInput << "strata = poor";
	V3::PopType poors(popTypeInput);
	poors.setType("poors");

	mappers::CultureDef french;
	french.religion = "sunni";
	french.name = "french";
	french.obsessions.insert("wine");

	mappers::ReligionDef sunni;
	sunni.name = "sunni";
	sunni.taboos.insert("wine");

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	market.calcPopOrders(53300, {{"poors", 1}}, {{"french", 1}}, defines, demand, {{"poors", poors}}, {}, {{"sunni", sunni}}, {}, {});

	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Culture: french has no definition. Assuming no obsessions or taboos.)"));
	EXPECT_THAT(market.getMarketBalance(),
		 testing::UnorderedElementsAre(testing::Pair("extra", testing::DoubleNear(0, 0.01)),
			  testing::Pair("grain", testing::DoubleNear(100 - 11.73, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(30 - 3.52, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(40 - 1.56, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(10 - 0.59, 0.01)),
			  testing::Pair("wine", testing::DoubleNear(20 - 0.78, 0.01))));
}

TEST(V3World_MarketTests, MissingReligionGivesWarning)
{
	// Input religion lacks a ReligionDef mapping. Expect to raise a Log(LogLevel::Warning) about this. Don't crash.
	V3::Market market({"grain", "fish", "meat", "groceries", "wine", "extra"});
	market.sell("grain", 100);
	market.sell("fish", 30);
	market.sell("meat", 40);
	market.sell("groceries", 10);
	market.sell("wine", 20);

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
	popNeedsInput << "entry = { goods = wine weight = 0.5 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = groceries weight = 0.75 max_supply_share = 0.9 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	buyPackagesInput << "wealth_8 = { goods = { popneed_basic_food = 118 } }\n";

	goodsInput << "grain = { cost = 20 }\n";
	goodsInput << "fish = { cost = 20 }\n";
	goodsInput << "meat = { cost = 30 }\n";
	goodsInput << "groceries = { cost = 30 }\n";
	goodsInput << "wine = { cost = 30 }\n";
	goodsInput << "extra = { cost = 90 }\n";

	demand.loadPopNeeds(popNeedsInput);
	demand.loadBuyPackages(buyPackagesInput);
	demand.loadGoods(goodsInput);

	std::stringstream popTypeInput;
	popTypeInput << "strata = poor";
	V3::PopType poors(popTypeInput);
	poors.setType("poors");

	mappers::CultureDef french;
	french.religion = "sunni";
	french.name = "french";
	french.obsessions.insert("wine");

	mappers::ReligionDef sunni;
	sunni.name = "sunni";
	sunni.taboos.insert("wine");

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());

	market.calcPopOrders(53300, {{"poors", 1}}, {{"french", 1}}, defines, demand, {{"poors", poors}}, {{"french", french}}, {}, {}, {});

	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Religion: sunni has no definition. Assuming no taboos.)"));
	EXPECT_THAT(market.getMarketBalance(),
		 testing::UnorderedElementsAre(testing::Pair("extra", testing::DoubleNear(0, 0.01)),
			  testing::Pair("grain", testing::DoubleNear(100 - 12.44, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(30 - 3.73, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(40 - 1.66, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(10 - 0.62, 0.01)),
			  testing::Pair("wine", testing::DoubleNear(20 - 3.32, 0.01))));
}

TEST(V3World_MarketTests, FormulaEstimatesCretianNeed)
{
	// Real test using 1.7* Crete.
	V3::Market market({
		 "grain",
		 "fish",
		 "meat",
		 "fruit",
		 "groceries",
		 "wine",
		 "opium",
		 "liquor",
		 "tobacco",
		 "wood",
		 "furniture",
		 "fabric",
		 "clothes",
		 "sugar",
		 "services",
		 "tea",
		 "coffee",
		 "sugar",
		 "small_arms",
		 "luxury_clothes",
		 "luxury_furniture",
		 "glass",
		 "paper",
		 "porcelain",
	});

	market.sell("fabric", 941);
	market.sell("grain", 845);
	market.sell("wood", 630);
	market.sell("fish", 483);
	market.sell("tobacco", 424);
	market.sell("sugar", 299);
	market.sell("clothes", 270);
	market.sell("meat", 140);
	market.sell("paper", 121);
	market.sell("coffee", 91);
	market.sell("furniture", 85);
	market.sell("liquor", 85);
	market.sell("opium", 73);
	market.sell("small_arms", 60);
	market.sell("fruit", 47.2);
	market.sell("porcelain", 45.4);
	market.sell("luxury_furniture", 35);
	market.sell("luxury_clothes", 22.5);
	market.sell("tea", 20.2);
	market.sell("glass", 20.2);
	market.sell("services", 2.27);

	market.buyForBuilding("fabric", 403);
	market.buyForBuilding("grain", 89);
	market.buyForBuilding("wood", 239);
	market.buyForBuilding("fish", 41.7);
	market.buyForBuilding("tobacco", 19.4);
	market.buyForBuilding("sugar", 34.7);
	market.buyForBuilding("clothes", 17.6);
	market.buyForBuilding("meat", 14);
	market.buyForBuilding("paper", 125.8);
	market.buyForBuilding("furniture", 13);
	market.buyForBuilding("liquor", 45.8);
	market.buyForBuilding("opium", 4.2);
	market.buyForBuilding("small_arms", 29.8);
	market.buyForBuilding("fruit", 2.1);

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

	popNeedsInput << "popneed_simple_clothing = { default = fabric\n";
	popNeedsInput << "entry = { goods = fabric weight = 1 max_supply_share = 0.5 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = clothes weight = 2 max_supply_share = 1.0 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	popNeedsInput << "popneed_crude_items = { default = wood\n";
	popNeedsInput << "entry = { goods = wood weight = 1 max_supply_share = 0.5 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = furniture weight = 2 max_supply_share = 1.0 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	popNeedsInput << "popneed_heating = { default = wood\n";
	popNeedsInput << "entry = { goods = wood weight = 0.75 max_supply_share = 0.5 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = fabric weight = 0.25 max_supply_share = 0.25 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	popNeedsInput << "popneed_household_items = { default = furniture\n";
	popNeedsInput << "entry = { goods = furniture weight = 1.0 max_supply_share = 0.75 min_supply_share = 0.1 }\n";
	popNeedsInput << "entry = { goods = glass weight = 1.0 max_supply_share = 0.5 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = paper weight = 0.5 max_supply_share = 0.5 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	popNeedsInput << "popneed_standard_clothing = { default = clothes\n";
	popNeedsInput << "entry = { goods = clothes }\n";
	popNeedsInput << "}\n";

	popNeedsInput << "popneed_services = { default = services\n";
	popNeedsInput << "entry = { goods = services }\n";
	popNeedsInput << "}\n";

	popNeedsInput << "popneed_intoxicants = { default = liquor\n";
	popNeedsInput << "entry = { goods = liquor weight = 1.0 max_supply_share = 0.75 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = tobacco weight = 1.0 max_supply_share = 0.75 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = opium weight = 1.5 max_supply_share = 0.75 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = wine weight = 0.25 max_supply_share = 0.25 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	popNeedsInput << "popneed_luxury_drinks = { default = tea\n";
	popNeedsInput << "entry = { goods = tea weight = 1.0 max_supply_share = 0.75 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = coffee weight = 1.0 max_supply_share = 0.75 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = wine weight = 0.33 max_supply_share = 0.33 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	popNeedsInput << "popneed_luxury_food = { default = meat\n";
	popNeedsInput << "entry = { goods = meat weight = 0.75 max_supply_share = 0.75 min_supply_share = 0.1 }\n";
	popNeedsInput << "entry = { goods = fruit weight = 0.75 max_supply_share = 0.75 min_supply_share = 0.1 }\n";
	popNeedsInput << "entry = { goods = groceries weight = 1.0 max_supply_share = 1.0 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = sugar weight = 0.5 max_supply_share = 0.5 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	popNeedsInput << "popneed_luxury_items = { default = luxury_clothes\n";
	popNeedsInput << "entry = { goods = luxury_clothes weight = 1.0 max_supply_share = 0.5 min_supply_share = 0.1 }\n";
	popNeedsInput << "entry = { goods = luxury_furniture weight = 1.0 max_supply_share = 0.5 min_supply_share = 0.1 }\n";
	popNeedsInput << "entry = { goods = porcelain weight = 0.33 max_supply_share = 0.5 min_supply_share = 0.1 }\n";
	popNeedsInput << "}\n";

	popNeedsInput << "popneed_leisure = { default = services\n";
	popNeedsInput << "entry = { goods = services weight = 0.1 max_supply_share = 1.0 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = small_arms weight = 0.75 max_supply_share = 0.25 min_supply_share = 0.0 }\n";
	popNeedsInput << "entry = { goods = opium weight = 0.5 max_supply_share = 0.5 min_supply_share = 0.0 }\n";
	popNeedsInput << "}\n";

	buyPackagesInput << "wealth_8 = { goods = { \n";
	buyPackagesInput << "popneed_basic_food = 121\n";
	buyPackagesInput << "popneed_crude_items = 39\n";
	buyPackagesInput << "popneed_heating = 27\n";
	buyPackagesInput << "popneed_intoxicants = 60\n";
	buyPackagesInput << "popneed_simple_clothing = 43\n";
	buyPackagesInput << "} }\n";

	buyPackagesInput << "wealth_17 = { goods = { \n";
	buyPackagesInput << "popneed_basic_food = 171\n";
	buyPackagesInput << "popneed_household_items = 105\n";
	buyPackagesInput << "popneed_heating = 26\n";
	buyPackagesInput << "popneed_intoxicants = 79\n";
	buyPackagesInput << "popneed_standard_clothing = 112\n";
	buyPackagesInput << "popneed_services = 86\n";
	buyPackagesInput << "popneed_luxury_drinks = 46\n";
	buyPackagesInput << "popneed_luxury_items = 39\n";
	buyPackagesInput << "} }\n";

	buyPackagesInput << "wealth_30 = { goods = { \n";
	buyPackagesInput << "popneed_luxury_food = 271\n";
	buyPackagesInput << "popneed_household_items = 209\n";
	buyPackagesInput << "popneed_heating = 26\n";
	buyPackagesInput << "popneed_intoxicants = 216\n";
	buyPackagesInput << "popneed_standard_clothing = 161\n";
	buyPackagesInput << "popneed_services = 397\n";
	buyPackagesInput << "popneed_luxury_drinks = 292\n";
	buyPackagesInput << "popneed_luxury_items = 522\n";
	buyPackagesInput << "popneed_leisure = 104\n";
	buyPackagesInput << "} }\n";


	goodsInput << "grain = { cost = 20 }\n";
	goodsInput << "wood = { cost = 20 }\n";
	goodsInput << "fabric = { cost = 20 }\n";
	goodsInput << "fish = { cost = 20 }\n";
	goodsInput << "meat = { cost = 30 }\n";
	goodsInput << "fruit = { cost = 30 }\n";
	goodsInput << "services = { cost = 30 local = yes }\n";
	goodsInput << "paper = { cost = 30 }\n";
	goodsInput << "groceries = { cost = 30 }\n";
	goodsInput << "furniture = { cost = 30 }\n";
	goodsInput << "clothes = { cost = 30 }\n";
	goodsInput << "wine = { cost = 50 }\n";
	goodsInput << "opium = { cost = 50 }\n";
	goodsInput << "porcelain = { cost = 70 }\n";
	goodsInput << "luxury_clothes = { cost = 60 }\n";
	goodsInput << "luxury_furniture = { cost = 60 }\n";
	goodsInput << "small_arms = { cost = 60 }\n";
	goodsInput << "tobacco = { cost = 40 }\n";
	goodsInput << "glass = { cost = 40 }\n";
	goodsInput << "sugar = { cost = 30 }\n";
	goodsInput << "coffee = { cost = 50 }\n";
	goodsInput << "tea = { cost = 50 }\n";
	goodsInput << "liquor = { cost = 30 }\n";

	demand.loadPopNeeds(popNeedsInput);
	demand.loadBuyPackages(buyPackagesInput);
	demand.loadGoods(goodsInput);

	std::stringstream poorsTypeInput;
	poorsTypeInput << "start_quality_of_life = 5";
	V3::PopType poors(poorsTypeInput);
	poors.setType("poors");

	std::stringstream peasantsTypeInput;
	peasantsTypeInput << "start_quality_of_life = 5\n";
	peasantsTypeInput << "consumption_mult = 0.05\n";
	V3::PopType peasants(peasantsTypeInput);
	peasants.setType("peasants");

	std::stringstream midTypeInput;
	midTypeInput << "start_quality_of_life = 10";
	V3::PopType mid(midTypeInput);
	mid.setType("mid");

	std::stringstream richTypeInput;
	richTypeInput << "start_quality_of_life = 20\n";
	richTypeInput << "working_adult_ratio = 0.2\n";
	V3::PopType rich(richTypeInput);
	rich.setType("rich");

	mappers::CultureDef turkish;
	turkish.religion = "sunni";
	turkish.obsessions.insert("tea");

	mappers::CultureDef greek;
	greek.religion = "orthodox";
	greek.obsessions.insert("wine");

	mappers::ReligionDef sunni;
	sunni.taboos.insert("wine");
	sunni.taboos.insert("liquor");

	mappers::ReligionDef orthodox;

	constexpr double size = 136344.0;

	market.calcPopOrders(size,
		 {
			  {"poors", 35904 / size},
			  {"peasants", 90800 / size},
			  {"mid", 8600 / size},
			  {"rich", 1040 / size},
		 },
		 {
			  {"greek", 78704 / size},
			  {"turkish", 59136 / size},
		 },
		 defines,
		 demand,
		 {
			  {"poors", poors},
			  {"peasants", peasants},
			  {"mid", mid},
			  {"rich", rich},
		 },
		 {
			  {"greek", greek},
			  {"turkish", turkish},
		 },
		 {
			  {"orthodox", orthodox},
			  {"sunni", sunni},
		 },
		 {},
		 {});

	EXPECT_THAT(market.getMarketBalance(),
		 testing::UnorderedElementsAre(testing::Pair("small_arms", testing::DoubleNear(30.16, 0.02)),
			  testing::Pair("clothes", testing::DoubleNear(248.3, 0.6)),
			  testing::Pair("coffee", testing::DoubleNear(90.6, 0.4)),
			  testing::Pair("fabric", testing::DoubleNear(534.7, 0.5)),
			  testing::Pair("fruit", testing::DoubleNear(44.8, 0.1)),
			  testing::Pair("furniture", testing::DoubleNear(69.8, 0.4)),
			  testing::Pair("fish", testing::DoubleNear(434, 0.6)),
			  testing::Pair("grain", testing::DoubleNear(743.9, 0.6)),
			  testing::Pair("glass", testing::DoubleNear(19.93, 0.15)),
			  testing::Pair("liquor", testing::DoubleNear(38.8, 0.25)),
			  testing::Pair("luxury_clothes", testing::DoubleNear(22.25, 0.1)),
			  testing::Pair("luxury_furniture", testing::DoubleNear(34.61, 0.1)),
			  testing::Pair("meat", testing::DoubleNear(125.3, 0.2)),
			  testing::Pair("opium", testing::DoubleNear(68.1, 0.1)),
			  testing::Pair("paper", testing::DoubleNear(-5.3266, 0.01)),
			  testing::Pair("porcelain", testing::DoubleNear(45.18, 0.1)),
			  testing::Pair("services", testing::DoubleNear(-0.45, 0.4)),
			  testing::Pair("sugar", testing::DoubleNear(263.9, 0.2)),
			  testing::Pair("tea", testing::DoubleNear(19.72, 0.25)),
			  testing::Pair("tobacco", testing::DoubleNear(401.8, 0.8)),
			  testing::Pair("wood", testing::DoubleNear(384.2, 0.25)),
			  testing::Pair("groceries", testing::DoubleNear(0, 0.01)),
			  testing::Pair("wine", testing::DoubleNear(-0.3, 0.3))));
}