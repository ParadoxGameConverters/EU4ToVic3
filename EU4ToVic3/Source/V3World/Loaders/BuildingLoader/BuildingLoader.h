#ifndef BUILDING_LOADER_H
#define BUILDING_LOADER_H
#include "EconomyManager/Building/Building.h"
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class BuildingLoader: commonItems::parser
{
  public:
	BuildingLoader() = default;

	void loadBuildings(const commonItems::ModFilesystem& modFS);
	[[nodiscard]] const auto& getBuildings() const { return buildings; }

  private:
	void registerKeys(const std::map<std::string, int>& costTiers);

	std::map<std::string, Building> buildings;
};
} // namespace V3
#endif // BUILDING_LOADER_H