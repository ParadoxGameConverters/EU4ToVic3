#ifndef V3_MARKET_TRACKER_H
#define V3_MARKET_TRACKER_H
#include "ClayManager/State/StateModifier.h"
#include "EconomyManager/Building/Building.h"
#include "EconomyManager/Building/BuildingGroups.h"
#include "EconomyManager/Building/BuildingResources.h"
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
		 double defaultRatio,
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, std::tuple<int, double>>& estimatedPMs,
		 const std::map<std::string, ProductionMethod>& PMs,
		 const std::map<std::string, ProductionMethodGroup>& PMGroups,
		 const std::map<std::string, PopType>& popTypes,
		 const std::map<std::string, Building>& buildings,
		 const BuildingGroups& buildingGroups,
		 const std::map<std::string, StateModifier>& stateTraits);
	void loadCultures(const std::map<std::string, double>& cultureData) { marketCulture = cultureData; }

	void updatePopNeeds(const Country& country,
		 const Vic3DefinesLoader& defines,
		 const DemandLoader& demandDefines,
		 const std::set<std::string>& laws,
		 const std::map<std::string, PopType>& popTypes,
		 const std::map<std::string, mappers::CultureDef>& cultures,
		 const std::map<std::string, mappers::ReligionDef>& religions,
		 const std::map<std::string, Law>& lawsMap);
	void integrateBuilding(const Building& building,
		 int p,
		 double defaultRatio,
		 const std::map<std::string, std::tuple<int, double>>& estimatedPMs,
		 const std::map<std::string, ProductionMethodGroup>& PMGroups,
		 const std::map<std::string, ProductionMethod>& PMs,
		 const BuildingGroups& buildingGroups,
		 const std::map<std::string, std::map<std::string, double>>& estOwnershipFractions,
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, Tech>& techMap,
		 const std::map<std::string, StateModifier>& stateTraits,
		 const std::map<std::string, PopType>& popTypes,
		 const std::map<std::string, Building>& buildings,
		 const std::shared_ptr<SubState>& subState);

	void logDebugMarket(const Country& country) const;

  private:
	static std::stringstream breakdownAsTable(const std::map<std::string, double>& breakdown, int popCount, bool asPercent = true);
	static double calcThroughputStateModifier(const std::vector<std::string>& traits,
		 const Building& building,
		 const BuildingGroups& buildingGroups,
		 const std::map<std::string, StateModifier>& stateTraits);
	static int calcEconomyOfScaleCap(const Country& country,
		 const Building& building,
		 const BuildingGroups& buildingGroups,
		 const std::map<std::string, Tech>& techMap);

	void updateMarketGoods(double level,
		 double p,
		 int eosCap,
		 double throughputMod,
		 const BuildingResources& buildingResources,
		 const std::vector<std::string>& traits,
		 const std::map<std::string, StateModifier>& stateTraits);


	Market market;
	MarketJobs marketJobs;
	std::map<std::string, double> marketCulture;

	static inline std::set<std::string> subsistenceErrors = {};
};
} // namespace V3

#endif // V3_MARKET_TRACKER_H