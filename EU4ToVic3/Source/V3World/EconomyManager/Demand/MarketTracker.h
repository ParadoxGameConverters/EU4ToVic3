#ifndef V3_MARKET_TRACKER_H
#define V3_MARKET_TRACKER_H
#include "EconomyManager/Building/ProductionMethods/ProductionMethod.h"
#include "EconomyManager/Building/ProductionMethods/ProductionMethodGroup.h"
#include "Market.h"
#include "MarketJobs.h"
#include "PoliticalManager/Country/Country.h"


namespace V3
{
class MarketTracker
{
  public:
	MarketTracker() = default;
	explicit MarketTracker(const std::map<std::string, Good>& possibleGoods,
		 const std::set<std::string>& manorHousePMGroups,
		 const std::map<std::string, ProductionMethodGroup>& PMGroups,
		 const std::map<std::string, ProductionMethod>& PMs);
	void resetMarket();
	void loadPeasants(const Country& country,
		 const std::map<std::string, ProductionMethodGroup>& PMGroups,
		 const std::map<std::string, ProductionMethod>& PMs,
		 const std::map<std::string, Building>& buildings,
		 const std::map<std::string, Law>& lawsMap);
	void loadCultures(const std::map<std::string, double> cultureData) { marketCulture = cultureData; }

	void updatePopNeeds(const Vic3DefinesLoader& defines,
		 const DemandLoader& demandDefines,
		 const std::set<std::string>& laws,
		 const std::map<std::string, PopType>& popTypes,
		 const std::map<std::string, mappers::CultureDef>& cultures,
		 const std::map<std::string, mappers::ReligionDef>& religions,
		 const std::map<std::string, Law>& lawsMap);

	void logDebugMarket();

  private:
	// Returns the index of the first valid PM according to laws.
	int getPMAllowedByLaws(const std::string& PMGroup,
		 const std::set<std::string>& laws,
		 const std::map<std::string, ProductionMethodGroup>& PMGroups,
		 const std::map<std::string, ProductionMethod>& PMs) const;
	bool hasUnlockingLaws(const std::set<std::string>& laws, const std::set<std::string>& targetLaws) const;
	bool hasBlockingLaws(const std::set<std::string>& laws, const std::set<std::string>& targetLaws) const;

	Market market;
	MarketJobs marketJobs;
	std::map<std::string, double> marketCulture;
};
} // namespace V3

#endif // V3_MARKET_TRACKER_H