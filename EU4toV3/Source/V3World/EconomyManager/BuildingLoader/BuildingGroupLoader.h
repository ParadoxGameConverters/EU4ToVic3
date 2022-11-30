#ifndef BUILDING_GROUP_LOADER_H
#define BUILDING_GROUP_LOADER_H
#include "BuildingGroups.h"
#include "Parser.h"


namespace V3
{
class BuildingGroupLoader: commonItems::parser
{
  public:
	BuildingGroupLoader() = default;

	void loadBuildingGroups(const std::string& v3Path);
	[[nodiscard]] std::shared_ptr<BuildingGroups> getBuildingGroups() { return buildingGroups; }


  private:
	void registerKeys();

	std::shared_ptr<BuildingGroups> buildingGroups;
};
} // namespace V3
#endif // BUILDING_GROUP_LOADER_H