#include "MarketTracker.h"

V3::MarketTracker::MarketTracker(const std::vector<std::string>& possibleGoods,
	 const std::vector<std::pair<std::string, int>>& manorHouseRoster)
{
	market = Market(possibleGoods);
	marketJobs = MarketJobs({}, manorHouseRoster);
}

void V3::MarketTracker::resetMarket()
{
	market.clearMarket();
	marketJobs.clearJobs();
}