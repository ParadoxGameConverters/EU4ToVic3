#ifndef BUILDING_GROUP_LOADER_H
#define BUILDING_GROUP_LOADER_H
#include "EconomyManager/Building/BuildingGroups.h"
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class BuildingGroupLoader: commonItems::parser
{
  public:
	BuildingGroupLoader() = default;

	void loadBuildingGroups(const commonItems::ModFilesystem& modFS);
	[[nodiscard]] const auto& getBuildingGroups() const { return buildingGroups; }

  private:
	void registerKeys();

	BuildingGroups buildingGroups;
};
} // namespace V3
#endif // BUILDING_GROUP_LOADER_H