#ifndef DEMAND_H
#define DEMAND_H
#include "EconomyManager/Demand/BuyPackage.h"
#include "EconomyManager/Demand/Good.h"
#include "EconomyManager/Demand/PopNeed.h"
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class DemandLoader: commonItems::parser
{
  public:
	DemandLoader() = default;

	void loadGoods(const commonItems::ModFilesystem& modFS);
	void loadGoods(std::istream& theStream);
	void loadPopNeeds(const commonItems::ModFilesystem& modFS);
	void loadPopNeeds(std::istream& theStream);
	void loadBuyPackages(const commonItems::ModFilesystem& modFS);
	void loadBuyPackages(std::istream& theStream);
	void cacheGoodsNeedsMap();

	[[nodiscard]] const auto& getGoodsMap() const { return goodsMap; }
	[[nodiscard]] const auto& getGoodsNeedsMap() const { return goodsNeedsMap; }
	[[nodiscard]] const auto& getPopneedsMap() const { return popneedsMap; }
	[[nodiscard]] const auto& getWealthConsumptionMap() const { return wealthConsumptionMap; }

  private:
	void registerKeys();

	std::map<std::string, Good> goodsMap;
	std::map<std::string, std::set<std::string>> goodsNeedsMap; // good -> the needs fulfilled by the good
	std::map<std::string, PopNeed> popneedsMap;						// name -> popneeds
	std::map<int, BuyPackage> wealthConsumptionMap;					// wealth level -> buy packages
};
} // namespace V3
#endif // DEMAND_H