#ifndef BUILDING_LOADER_H
#define BUILDING_LOADER_H

#include "Parser.h"

namespace V3
{
class Building;
class BuildingLoader: commonItems::parser
{
  public:
	BuildingLoader() = default;

	void loadBuildings(const commonItems::ModFilesystem& modFS);
	[[nodiscard]] const auto& getBuildings() const { return buildings; }

  private:
	void registerKeys(const std::map<std::string, int>& costTiers);

	std::map<std::string, std::shared_ptr<Building>> buildings;
};
} // namespace V3
#endif // BUILDING_LOADER_H