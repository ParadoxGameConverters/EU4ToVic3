#include "Loaders/DemandLoader/DemandLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game", {});
}

TEST(V3World_DemandLoaderTests, DemandLoaderCanLoadDemandComponents)
{
	V3::DemandLoader demandLoader;
	EXPECT_TRUE(demandLoader.getGoodsMap().empty());
	EXPECT_TRUE(demandLoader.getGoodsNeedsMap().empty());
	EXPECT_TRUE(demandLoader.getPopneedsMap().empty());
	EXPECT_TRUE(demandLoader.getWealthConsumptionMap().empty());

	demandLoader.loadGoods(modFS);
	demandLoader.loadPopNeeds(modFS);
	demandLoader.loadBuyPackages(modFS);

	const std::map<std::string, std::set<std::string>> goodsNeedsMap = {
		 {"fabric", std::set<std::string>{"popneed_simple_clothing"}},
		 {"clothes", std::set<std::string>{"popneed_simple_clothing", "popneed_flame_items"}},
		 {"wood", std::set<std::string>{"popneed_flame_items"}},
	};

	EXPECT_EQ(goodsNeedsMap, demandLoader.getGoodsNeedsMap());
	EXPECT_EQ(3, demandLoader.getGoodsMap().size());
	EXPECT_EQ(2, demandLoader.getPopneedsMap().size());
	EXPECT_EQ(2, demandLoader.getWealthConsumptionMap().size());

	EXPECT_EQ(30, demandLoader.getGoodsMap().at("paper").getBasePrice());
	EXPECT_EQ(30, demandLoader.getGoodsMap().at("services").getBasePrice());
	EXPECT_FALSE(demandLoader.getGoodsMap().at("paper").isLocal());
	EXPECT_TRUE(demandLoader.getGoodsMap().at("services").isLocal());
}