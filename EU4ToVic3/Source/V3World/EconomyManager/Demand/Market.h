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
	void loadGoods(const std::map<std::string, Good>& goodsList);

	[[nodiscard]] std::map<std::string, double> getMarketBalance() const;
	[[nodiscard]] std::map<std::string, double> getMarketShare(const std::vector<std::string>& goods) const;

	void sell(const std::string& good, double amount);
	void buyForBuilding(const std::string& good, double amount);
	void buyForPop(const std::string& good, double amount); // Public for Debug purposes
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
	void clearMarket();
	std::stringstream marketAsTable() const;

	static double calcAddedWorkingPopFraction(const std::set<std::string>& laws, const std::map<std::string, Law>& lawsMap);

  private:
	static int estimateWealth(int startingQOL);
	static std::set<std::string> getObsessions(const std::string& culture, const std::map<std::string, mappers::CultureDef>& cultures);
	static std::set<std::string> getTaboos(const std::string& culture,
		 const std::map<std::string, mappers::CultureDef>& cultures,
		 const std::map<std::string, mappers::ReligionDef>& religions);
	static double calcCulturalFactor(double culturalPrevalence);
	static double calcPopFactor(double size,
		 const PopType& popType,
		 const Vic3DefinesLoader& defines,
		 const std::set<std::string>& laws,
		 const std::map<std::string, Law>& lawsMap);
	static std::map<std::string, double> calcPurchaseWeights(const std::map<std::string, double>& marketShareMap,
		 const std::map<std::string, GoodsFulfillment>& fulfillments,
		 const std::map<std::string, double>& culturalPrevalence,
		 const std::map<std::string, Good>& goodsMap);
	static double calcPurchaseWeight(double marketShare, const GoodsFulfillment& fulfillment, double culturalPrevalence);
	static double calcCulturalNeedFactor(const std::vector<std::string>& goods, const std::map<std::string, double>& culturalPrevalence);

	[[nodiscard]] bool validateGood(const std::string& good) const;
	[[nodiscard]] std::vector<std::string> enumerateGoods(const std::map<std::string, GoodsFulfillment>& map, const std::map<std::string, Good>& goodsMap) const;
	[[nodiscard]] std::map<std::string, double> initCulturalFactors() const;

	// How much the market leans toward a certain good being a taboo or an obsession.
	std::map<std::string, double> estimateCulturalPrevalence(const std::map<std::string, double>& cultureData,
		 const std::map<std::string, mappers::CultureDef>& cultures,
		 const std::map<std::string, mappers::ReligionDef>& religions) const;

	std::map<std::string, double> sellOrders;
	std::map<std::string, double> buyOrdersBuildings;
	std::map<std::string, double> buyOrdersPops;

	static inline std::set<std::string> popTypeErrors = {};
	static inline std::set<std::string> goodsErrors = {};
	static inline std::set<std::string> popneedsErrors = {};
	static inline std::set<std::string> cultureErrors = {};
	static inline std::set<std::string> religionErrors = {};
	static inline std::set<int> wealthErrors = {};
};
} // namespace V3

#endif // V3_MARKET_H