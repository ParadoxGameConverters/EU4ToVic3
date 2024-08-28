#ifndef V3_MARKET_TRACKER_H
#define V3_MARKET_TRACKER_H
#include "CultureMapper/CultureDefinitionLoader/CultureDef.h"
#include "Loaders/DefinesLoader/Vic3DefinesLoader.h"
#include "Loaders/DemandLoader/DemandLoader.h"
#include "Loaders/LawLoader/Law.h"
#include "Market.h"
#include "MarketJobs.h"
#include "PopManager/Pops/PopType.h"
#include "ReligionMapper/ReligionDefinitionLoader/ReligionDef.h"
#include "string"

namespace V3
{
class MarketTracker
{
  public:
	MarketTracker() = default;
	explicit MarketTracker(const std::vector<std::string>& possibleGoods, const std::vector<std::pair<std::string, int>>& manorHouseRoster);

	void resetMarket();

  private:
	Market market;
	MarketJobs marketJobs;
};
} // namespace V3

#endif // V3_MARKET_TRACKER_H