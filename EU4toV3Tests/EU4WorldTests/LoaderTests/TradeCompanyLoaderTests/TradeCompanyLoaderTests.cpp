#include "TradeCompanyLoader/TradeCompanyLoader.h"
#include "gtest/gtest.h"

namespace
{
auto modFS = commonItems::ModFilesystem("TestFiles/eu4installation", {});
}

TEST(EU4World_TradeCompanyLoaderTests, TradeCompanyProvincesCanBePinged)
{
	EU4::TradeCompanyLoader loader;
	loader.loadTradeCompanies(modFS);

	EXPECT_TRUE(loader.isProvinceInTradeRegion(1164));
	EXPECT_TRUE(loader.isProvinceInTradeRegion(518));
	EXPECT_FALSE(loader.isProvinceInTradeRegion(9999));
}

TEST(EU4World_TradeCompanyLoaderTests, TradeCompanyProvincesCanBeExcluded)
{
	EU4::TradeCompanyLoader loader;
	loader.loadTradeCompanies(modFS);

	EXPECT_TRUE(loader.isProvinceInTradeRegion(1164));
	EXPECT_TRUE(loader.isProvinceInTradeRegion(518));

	loader.loadExcludedTradeCompanies("TestFiles/configurables/excluded_trade_companies.txt");

	EXPECT_FALSE(loader.isProvinceInTradeRegion(1164));
	EXPECT_TRUE(loader.isProvinceInTradeRegion(518));
	EXPECT_FALSE(loader.isProvinceInTradeRegion(9999));
}

TEST(EU4World_TradeCompanyLoaderTests, TradeCompaniesCanBeRetrieved)
{
	EU4::TradeCompanyLoader loader;
	loader.loadTradeCompanies(modFS);
	loader.loadExcludedTradeCompanies("TestFiles/configurables/excluded_trade_companies.txt");
	auto tc1 = loader.getTCForProvince(1164);
	auto tc2 = loader.getTCForProvince(518);
	auto tc3 = loader.getTCForProvince(9999);

	EXPECT_FALSE(tc1);
	EXPECT_TRUE(tc2);
	EXPECT_FALSE(tc3);

	EXPECT_EQ("trade_company_west_india", (*tc2).name);
}
