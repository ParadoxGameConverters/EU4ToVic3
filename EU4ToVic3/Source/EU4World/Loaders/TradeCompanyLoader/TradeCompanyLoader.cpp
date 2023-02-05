#include "TradeCompanyLoader.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "TradeCompanyEntry.h"
#include <algorithm>
#include <ranges>

void EU4::TradeCompanyLoader::loadTradeCompanies(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading Trade Company Regions.";
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("common/trade_companies"))
		parseFile(fileName);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << tradingCompanies.size() << " Trade Companies.";
}

void EU4::TradeCompanyLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& tcName, std::istream& theStream) {
		auto tc = TradeCompanyEntry(theStream).getTradeCompany();
		tc.name = tcName;
		tradingCompanies.emplace(tcName, tc);
	});
}

bool EU4::TradeCompanyLoader::isProvinceInTradeRegion(int province) const
{
	return std::ranges::any_of(tradingCompanies, [&](const auto& tradeCompany) {
		return tradeCompany.second.provinces.contains(province) && !excludedTCMapper.getExcludedTCs().contains(tradeCompany.second.name);
	});
}

std::optional<EU4::TradeCompany> EU4::TradeCompanyLoader::getTCForProvince(int province) const
{
	for (const auto& tc: tradingCompanies | std::views::values)
		if (tc.provinces.contains(province) && !excludedTCMapper.getExcludedTCs().contains(tc.name))
			return tc;
	return std::nullopt;
}
