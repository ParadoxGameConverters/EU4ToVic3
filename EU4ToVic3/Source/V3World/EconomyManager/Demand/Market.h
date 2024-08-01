#ifndef V3_MARKET_H
#define V3_MARKET_H
#include "string"

namespace V3
{
class Market
{
  public:
	Market() = default;

	[[nodiscard]] const std::map<std::string, double> getMarketBalance() const;
	[[nodiscard]] const std::map<std::string, double> getMarketShare(const std::set<std::string>& goods) const;

	void sell(const std::string& good, double amount) { sellOrders.at(good) += amount; };
	void buyForBuilding(const std::string& good, double amount) { buyOrdersBuildings.at(good) += amount; };
	void buyForPop(const std::string& good, double amount) { buyOrdersPops.at(good) += amount; };

  private:
	std::map<std::string, double> sellOrders;
	std::map<std::string, double> buyOrdersBuildings;
	std::map<std::string, double> buyOrdersPops;
};
} // namespace V3

#endif // V3_MARKET_H