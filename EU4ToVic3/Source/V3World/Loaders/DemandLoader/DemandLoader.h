#ifndef DEMAND_H
#define DEMAND_H
#include "EconomyManager/Demand/BuyPackage.h"
#include "EconomyManager/Demand/PopNeed.h"
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class DemandLoader: commonItems::parser
{
  public:
	DemandLoader() = default;

	void loadPopNeeds(const commonItems::ModFilesystem& modFS);
	void loadBuyPackages(const commonItems::ModFilesystem& modFS);
	void cacheGoodsNeedsMap();

	[[nodiscard]] const auto& getGoodsNeedsMap() { return goodsNeedsMap; }
	[[nodiscard]] const auto& getPopneedsMap() { return popneedsMap; }
	[[nodiscard]] const auto& getWealthConsumptionMap() { return wealthConsumptionMap; }

  private:
	void registerKeys();

	std::map<std::string, std::set<std::string>> goodsNeedsMap; // good -> the needs fulfilled by the good
	std::map<std::string, PopNeed> popneedsMap;						// name -> popneeds
	std::map<int, BuyPackage> wealthConsumptionMap;					// wealth level -> buy packages
};
} // namespace V3
#endif // DEMAND_H