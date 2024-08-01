#include "Market.h"
#include "pch.h"

const std::map<std::string, double> V3::Market::getMarketBalance() const
{
	std::map<std::string, double> marketBalance;
	for (const auto& [good, amount]: sellOrders)
	{
		marketBalance[good] = amount - buyOrdersBuildings.at(good) - buyOrdersPops.at(good);
	}

	return marketBalance;
}

const std::map<std::string, double> V3::Market::getMarketShare(const std::set<std::string>& goods) const
{
	double totalAdjustedOrders = 0;
	std::map<std::string, double> adjustedOrders;

	for (const std::string& good: goods)
	{
		const double adjustedOrder = sellOrders.at(good) - 0.5 * buyOrdersBuildings.at(good);
		totalAdjustedOrders += adjustedOrder;
		adjustedOrders[good] = adjustedOrder;
	}

	std::map<std::string, double> marketShare;
	for (const auto& [good, amount]: adjustedOrders)
	{
		marketShare[good] = amount / totalAdjustedOrders;
	}
}
