#ifndef BUILDING_COST_LOADER
#define BUILDING_COST_LOADER
#include "BuildingCostLoader.h"
#include "ModLoader/ModLoader.h"
#include "Parser.h"
#include <map>
#include <optional>

namespace EU4
{
class BuildingCostLoader: commonItems::parser
{
  public:
	BuildingCostLoader() = default;

	void loadBuildingCosts(const std::string& EU4Path, const Mods& mods);
	void loadBuildingCosts(std::istream& theStream);

	[[nodiscard]] std::optional<double> getBuildingCost(const std::string& buildingName) const;

  private:
	void registerKeys();

	std::map<std::string, double> buildingCosts;
};
} // namespace EU4

#endif // BUILDING_COST_LOADER