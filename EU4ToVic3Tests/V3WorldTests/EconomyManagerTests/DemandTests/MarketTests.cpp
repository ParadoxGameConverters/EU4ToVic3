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

TEST(V3World_MarketTests, FormulaPredictsPeasantsNeed)
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
	popTypeInput << "strata = rich\n";
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
	// Single Pop, Single Need: Poor Food.
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

	buyPackagesInput << "wealth_7 = { goods = { popneed_basic_food = 118 } }\n";

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
	none.taboos.insert("grain");
	none.taboos.insert("fish");

	market.calcPopOrders(53300, {{"poors", 1}}, {{"english", 1}}, defines, demand, {{"poors", poors}}, {{"english", english}}, {{"none", none}}, {}, {});
	EXPECT_THAT(market.getMarketBalance(),
		 testing::UnorderedElementsAre(testing::Pair("extra", testing::DoubleNear(0, 0.01)),
			  testing::Pair("grain", testing::DoubleNear(100 - 4.79, 0.01)),
			  testing::Pair("fish", testing::DoubleNear(30 - 1.44, 0.01)),
			  testing::Pair("meat", testing::DoubleNear(40 - 1.28, 0.01)),
			  testing::Pair("groceries", testing::DoubleNear(10 - 0.48, 0.01)),
			  testing::Pair("fruit", testing::DoubleNear(20 - 0.64, 0.01))));
}


TEST(V3World_MarketTests, FormulaPredictsObsessionsAndTaboosNeed)
{
	// Single Pop, Single Need: Poor Food.
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

	buyPackagesInput << "wealth_7 = { goods = { popneed_basic_food = 118 } }\n";

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
	french.religion = "none";
	french.name = "french";

	mappers::ReligionDef none;
	none.name = "none";
	none.taboos.insert("wine");

	market.calcPopOrders(53300, {{"poors", 1}}, {{"french", 1}}, defines, demand, {{"poors", poors}}, {{"french", french}}, {{"none", none}}, {}, {});
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

// Test Local Goods
// Test 0s
// Test nation-wide
// Test multi-package