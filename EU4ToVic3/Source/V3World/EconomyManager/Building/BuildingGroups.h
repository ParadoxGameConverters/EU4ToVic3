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
	void addBuildingGroup(const std::shared_ptr<BuildingGroup>& theBuildingGroup);
	void setInfrastructureCosts();
	void setResourceCaps();

	[[nodiscard]] std::optional<std::string> getAncestralCategory(const std::optional<std::string>& theGroupName) const;
	[[nodiscard]] const auto& getBuildingGroupMap() const { return buildingGroups; }
	[[nodiscard]] std::optional<std::string> tryGetParentName(const std::optional<std::string>& theGroupName) const;
	[[nodiscard]] std::optional<double> tryGetInfraCost(const std::optional<std::string>& theGroupName) const;
	[[nodiscard]] std::optional<bool> tryGetIsCapped(const std::optional<std::string>& theGroupName) const;
	[[nodiscard]] bool usesArableLand(const std::optional<std::string>& theGroupName) const;

  private:
	std::map<std::string, std::shared_ptr<BuildingGroup>> buildingGroups;
};
} // namespace V3

#endif // V3_BUILDING_GROUPS_H
