#ifndef V3_BUILDING_GROUPS_H
#define V3_BUILDING_GROUPS_H
#include "BuildingGroup.h"
#include "Parser.h"
#include <map>

namespace V3
{
class BuildingGroups
{
  public:
	BuildingGroups() = default;
	void addBuildingGroup(std::shared_ptr<BuildingGroup> theBuildingGroup) { buildingGroups[theBuildingGroup->getName()] = theBuildingGroup; }
	void setInfrastructureCosts();

	[[nodiscard]] const auto& getBuildingGroupMap() const { return buildingGroups; }
	[[nodiscard]] const std::string& getParentName(const std::string& theBuildingGroupName) const;
	[[nodiscard]] const std::string& safeGetParentName(const std::string& theBuildingGroupName) const;
	[[nodiscard]] const int getInfrastructureCost(const std::string& theBuildingGroupName) const;
	[[nodiscard]] const int safeGetInfrastructureCost(const std::string& theBuildingGroupName) const;


  private:
	const std::shared_ptr<BuildingGroup> safeGetBuildingGroup(const std::string& theBuildingGroupName) const;

	std::map<std::string, std::shared_ptr<BuildingGroup>> buildingGroups;
};
} // namespace V3

#endif // V3_BUILDING_GROUPS_H