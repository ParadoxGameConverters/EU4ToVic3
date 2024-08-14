#ifndef V3_MARKET_H
#define V3_MARKET_H
#include "CultureMapper/CultureDefinitionLoader/CultureDef.h"
#include "Loaders/DefinesLoader/Vic3DefinesLoader.h"
#include "Loaders/DemandLoader/DemandLoader.h"
#include "Loaders/LawLoader/Law.h"
#include "PopManager/Pops/PopType.h"
#include "ReligionMapper/ReligionDefinitionLoader/ReligionDef.h"
#include "string"

namespace V3
{
class Market
{
  public:
	Market() = default;
	explicit Market(const std::vector<std::string>& possibleGoods);

	[[nodiscard]] std::map<std::string, double> getMarketBalance() const;
	[[nodiscard]] std::map<std::string, double> getMarketShare(const std::vector<std::string>& goods) const;

	void sell(const std::string& good, const double amount) { sellOrders.at(good) += amount; }
	void buyForBuilding(const std::string& good, double const amount) { buyOrdersBuildings.at(good) += amount; }
	void buyForPop(const std::string& good, double const amount) { buyOrdersPops.at(good) += amount; }
	void calcPopOrders(int popSize,
		 const std::map<std::string, double>& jobData,
		 const std::map<std::string, double>& cultureData,
		 const Vic3DefinesLoader& defines,
		 const DemandLoader& demand,
		 const std::map<std::string, PopType>& popTypeMap,
		 const std::map<std::string, mappers::CultureDef>& cultures,
		 const std::map<std::string, mappers::ReligionDef>& religions,
		 const std::set<std::string>& laws,
		 const std::map<std::string, Law>& lawsMap);

  private:
	std::map<std::string, double> sellOrders;
	std::map<std::string, double> buyOrdersBuildings;
	std::map<std::string, double> buyOrdersPops;

	static int estimateWealth(const std::string& strata);
	static std::set<std::string> getObsessions(const std::string& culture, const std::map<std::string, mappers::CultureDef>& cultures);
	static std::set<std::string> getTaboos(const std::string& culture,
		 const std::map<std::string, mappers::CultureDef>& cultures,
		 const std::map<std::string, mappers::ReligionDef>& religions);
	static std::vector<std::string> enumerateGoods(const std::map<std::string, GoodsFulfillment>& map);
	static std::map<std::string, double> initCulturalFactors(const std::map<std::string, Good>& goodsMap);
	static std::map<std::string, double> estimateCulturalPrevalence(const std::map<std::string, double>& cultureData,
		 const std::map<std::string, mappers::CultureDef>& cultures,
		 const std::map<std::string, mappers::ReligionDef>& religions,
		 const std::map<std::string, Good>& goodsMap); // How much the market leans toward a certain good being a taboo or an obsession.
	static double calcCulturalFactor(double culturalPrevalence);
	static double calcPopFactor(double size,
		 const PopType& popType,
		 const Vic3DefinesLoader& defines,
		 const std::set<std::string>& laws,
		 const std::map<std::string, Law>& lawsMap);
	static std::map<std::string, double> calcPurchaseWeights(const std::map<std::string, double>& marketShareMap,
		 const std::map<std::string, GoodsFulfillment>& fulfillments,
		 const std::map<std::string, double>& culturalPrevalence);
	static double calcPurchaseWeight(const std::string& goodName, double marketShare, const GoodsFulfillment& fulfillment, double culturalPrevalence);
	static double calcCulturalNeedFactor(const std::vector<std::string>& goods, const std::map<std::string, double>& culturalPrevalence);
	static double calcAddedWorkingPopPercent(const std::set<std::string>& laws, const std::map<std::string, Law>& lawsMap);
};
} // namespace V3

#endif // V3_MARKET_H