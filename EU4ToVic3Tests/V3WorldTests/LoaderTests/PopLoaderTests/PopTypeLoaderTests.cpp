#include "Loaders/PopLoader/PopTypeLoader.h";
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
}

TEST(V3World_PopTypeLoaderTests, PopTypeLoaderCanLoadPopTypeComponents)
{
	V3::PopTypeLoader popTypeLoader;
	EXPECT_TRUE(popTypeLoader.getPopTypes().empty());

	popTypeLoader.loadPopTypes(modFS);

	const std::map<std::string, std::set<std::string>> goodsNeedsMap = {
		 {"fabric", std::set<std::string>{"popneed_simple_clothing"}},
		 {"clothes", std::set<std::string>{"popneed_simple_clothing", "popneed_flame_items"}},
		 {"wood", std::set<std::string>{"popneed_flame_items"}},
	};

	EXPECT_EQ(2, popTypeLoader.getPopTypes().size());
	EXPECT_EQ("peasants", popTypeLoader.getPopTypes().at("peasants").getType());
	EXPECT_DOUBLE_EQ(0.05, popTypeLoader.getPopTypes().at("peasants").getConsumptionRate());
	EXPECT_EQ(std::nullopt, popTypeLoader.getPopTypes().at("peasants").getDependentRatio());
	EXPECT_EQ("poor", popTypeLoader.getPopTypes().at("peasants").getStrata());
	EXPECT_EQ("aristocrats", popTypeLoader.getPopTypes().at("aristocrats").getType());
	EXPECT_DOUBLE_EQ(1.0, popTypeLoader.getPopTypes().at("aristocrats").getConsumptionRate());
	EXPECT_DOUBLE_EQ(0.2, popTypeLoader.getPopTypes().at("aristocrats").getDependentRatio().value());
	EXPECT_EQ("rich", popTypeLoader.getPopTypes().at("aristocrats").getStrata());
}