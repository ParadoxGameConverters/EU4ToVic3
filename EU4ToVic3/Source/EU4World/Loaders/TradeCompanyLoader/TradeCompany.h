#ifndef TRADE_COMPANY
#define TRADE_COMPANY
#include "Color.h"
#include <set>
#include <string>
#include <vector>

namespace EU4
{
struct TradeCompany
{
	std::string name;
	std::optional<commonItems::Color> color;
	std::set<int> provinces;
	std::vector<std::string> locNameKeys; // composite locnameKeys that need to be processed! Fist one is specific, second is generic.
};
} // namespace EU4

#endif // TRADE_COMPANY
