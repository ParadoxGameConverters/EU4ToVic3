#include "TradeCompanyLoader/TradeCompanyEntry.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(EU4World_TradeCompanyEntryTests, tradeCompanyDefaultsToEmpty)
{
	const EU4::TradeCompanyEntry entry;
	const auto& tc = entry.getTradeCompany();

	EXPECT_TRUE(tc.name.empty());
	EXPECT_FALSE(tc.color);
	EXPECT_TRUE(tc.provinces.empty());
	EXPECT_TRUE(tc.locNameKeys.empty());
}

TEST(EU4World_TradeCompanyEntryTests, tradeCompanyCanBeLoaded)
{
	std::stringstream input;
	input << "color = { 135 104 85 }\n";
	input << "provinces = { 135 104 }\n";
	input << "names = {\n";
	input << "	trigger = { some junk }\n"; // skipping this.
	input << "	name = \"TRADE_COMPANY_INDIA_British_trade_company_south_india\"\n ";
	input << "}\n";
	input << "names = {\n";
	input << "	name = \"TRADE_COMPANY_WEST_INDIA_Root_Culture_GetName\"\n ";
	input << "}\n";
	input << "names = {\n";
	input << "	name = \"TRADE_COMPANY_WEST_INDIA_India_Trade_Company\"\n ";
	input << "}\n";
	const EU4::TradeCompanyEntry entry(input);
	const auto& tc = entry.getTradeCompany();

	EXPECT_TRUE(tc.name.empty());
	EXPECT_EQ(commonItems::Color(std::array{135, 104, 85}), tc.color);
	EXPECT_THAT(tc.provinces, testing::UnorderedElementsAre(135, 104));
	EXPECT_THAT(tc.locNameKeys, testing::UnorderedElementsAre("TRADE_COMPANY_WEST_INDIA_Root_Culture_GetName", "TRADE_COMPANY_WEST_INDIA_India_Trade_Company"));
}
