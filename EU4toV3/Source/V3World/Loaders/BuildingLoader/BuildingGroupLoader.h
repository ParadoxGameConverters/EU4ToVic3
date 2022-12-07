#ifndef BUILDING_GROUP_LOADER_H
#define BUILDING_GROUP_LOADER_H
#include "Parser.h"

namespace V3
{
class BuildingGroups;
class BuildingGroupLoader: commonItems::parser
{
  public:
	BuildingGroupLoader();

	void loadBuildingGroups(const commonItems::ModFilesystem& modFS);
	[[nodiscard]] std::shared_ptr<BuildingGroups> getBuildingGroups() const { return buildingGroups; }

  private:
	void registerKeys();

	std::shared_ptr<BuildingGroups> buildingGroups;
};
} // namespace V3
#endif // BUILDING_GROUP_LOADER_H