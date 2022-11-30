#include "BuildingGroups.h"
#include "Log.h"
#include <ranges>

void V3::BuildingGroups::setInfrastructureCosts()
{
	for (auto& buildingGroup: std::views::values(buildingGroups))
	{
		std::string parentGroupName = buildingGroup->getParent();
		while (buildingGroup->getInfrastructureCost() == 0 && !parentGroupName.empty())
		{
			buildingGroup->setInfrastructureCost(getInfrastuctureCost(parentGroupName));
			parentGroupName = getParent(parentGroupName);
		}
	}
}

const std::string& V3::BuildingGroups::getParent(const std::string& theBuildingGroupName) const
{
	return getBuildingGroup(theBuildingGroupName)->getParent();
}

const int V3::BuildingGroups::getInfrastuctureCost(const std::string& theBuildingGroupName) const
{
	return getBuildingGroup(theBuildingGroupName)->getInfrastructureCost();
}

const std::shared_ptr<V3::BuildingGroup> V3::BuildingGroups::getBuildingGroup(const std::string& theBuildingGroupName) const
{
	const auto& possibleBuildingGroup = buildingGroups.find(theBuildingGroupName);
	if (possibleBuildingGroup != buildingGroups.end())
	{
		return possibleBuildingGroup->second;
	}
	else
	{
		Log(LogLevel::Error) << "Key not recognized: " << theBuildingGroupName << " is not a recognized building_group. Using an empty group in its place.";
		return std::move(std::make_unique<BuildingGroup>());
	}
}