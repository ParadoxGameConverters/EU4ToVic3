#ifndef V3_BUILDING_GROUPS_H
#define V3_BUILDING_GROUPS_H
#include "BuildingGroup.h"
#include "Parser.h"
#include <map>

/* A Chunk is a collection of provinces defined by province_mappings.txt where a particular eu4 province maps to.
 * Its defining attribute is the single owner these provinces belong to. From Chunks we can generate substates, when
 * we overlay this political map over a geographical map.
 *
 * A chunk can be as small as a single V3 province, or as large as multiple V3 states.*/

namespace V3
{
class BuildingGroups
{
  public:
	BuildingGroups() = default;
	void addBuildingGroup(std::shared_ptr<BuildingGroup> theBuildingGroup) { buildingGroups[theBuildingGroup->getName()] = theBuildingGroup; }
	void setInfrastructureCosts();

	[[nodiscard]] const auto& getBuildingGroupMap() const { return buildingGroups; }
	[[nodiscard]] const std::string& getParent(const std::string& theBuildingGroupName) const;
	[[nodiscard]] const int getInfrastuctureCost(const std::string& theBuildingGroupName) const;


  private:
	const std::shared_ptr<BuildingGroup> getBuildingGroup(const std::string& theBuildingGroupName) const;

	std::map<std::string, std::shared_ptr<BuildingGroup>> buildingGroups;
};
} // namespace V3

#endif // V3_BUILDING_GROUPS_H