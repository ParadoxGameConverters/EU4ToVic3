#ifndef V3_BUILDING_GROUPS_H
#define V3_BUILDING_GROUPS_H
#include "Parser.h"

namespace V3
{
class BuildingGroup;
class BuildingGroups
{
  public:
	BuildingGroups() = default;
	void addBuildingGroup(std::shared_ptr<BuildingGroup> theBuildingGroup);
	void setInfrastructureCosts();

	[[nodiscard]] const auto& getBuildingGroupMap() const { return buildingGroups; }
	[[nodiscard]] const std::string& getParentName(const std::string& theBuildingGroupName) const;
	[[nodiscard]] const std::string& safeGetParentName(const std::string& theBuildingGroupName) const;
	[[nodiscard]] int getInfrastructureCost(const std::string& theBuildingGroupName) const;
	[[nodiscard]] int safeGetInfrastructureCost(const std::string& theBuildingGroupName) const;


  private:
	[[nodiscard]] std::shared_ptr<BuildingGroup> safeGetBuildingGroup(const std::string& theBuildingGroupName) const;

	std::map<std::string, std::shared_ptr<BuildingGroup>> buildingGroups;
};
} // namespace V3

#endif // V3_BUILDING_GROUPS_H