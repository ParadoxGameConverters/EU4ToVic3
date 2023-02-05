#ifndef TRADE_COMPANY_LOADER
#define TRADE_COMPANY_LOADER
#include "ExcludedTradeCompaniesMapper/ExcludedTradeCompaniesMapper.h"
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include "TradeCompany.h"

namespace EU4
{
class TradeCompanyLoader: commonItems::parser
{
  public:
	TradeCompanyLoader() = default;
	void loadTradeCompanies(const commonItems::ModFilesystem& modFS);
	void loadExcludedTradeCompanies(const std::string& filePath) { excludedTCMapper.loadMappingRules(filePath); }

	[[nodiscard]] bool isProvinceInTradeRegion(int province) const;
	[[nodiscard]] std::optional<TradeCompany> getTCForProvince(int province) const;
	[[nodiscard]] const auto& getTradeCompany(const std::string& name) const { return tradingCompanies.at(name); }

  private:
	void registerKeys();

	std::map<std::string, TradeCompany> tradingCompanies;
	mappers::ExcludedTradeCompaniesMapper excludedTCMapper;
};
} // namespace EU4

#endif // TRADE_COMPANY_LOADER
