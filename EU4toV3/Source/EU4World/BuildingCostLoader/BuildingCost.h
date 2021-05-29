#ifndef BUILDING_COST
#define BUILDING_COST
#include "ConvenientParser.h"

namespace EU4
{
class BuildingCost: commonItems::convenientParser
{
  public:
	explicit BuildingCost(std::istream& theStream);

	[[nodiscard]] auto getCost() const { return cost; }

  private:
	void registerKeys();

	double cost = 0;
};
} // namespace EU4

#endif // EU4