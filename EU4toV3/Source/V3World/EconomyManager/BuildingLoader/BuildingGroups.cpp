#include "BuildingGroups.h"
#include "Log.h"
#include <ranges>

void V3::BuildingGroups::setInfrastructureCosts()
{
	for (auto& buildingGroup: std::views::values(buildingGroups))
	{
		std::string parentGroupName = buildingGroup->getParentName();
		while (buildingGroup->getInfrastructureCost() == 0 && !parentGroupName.empty())
		{
			buildingGroup->setInfrastructureCost(safeGetInfrastructureCost(parentGroupName));
			parentGroupName = safeGetParentName(parentGroupName);
		}
	}
}

const std::string& V3::BuildingGroups::getParentName(const std::string& theBuildingGroupName) const
{
	return buildingGroups.at(theBuildingGroupName)->getParentName();
}

const std::string& V3::BuildingGroups::safeGetParentName(const std::string& theBuildingGroupName) const
{
	return safeGetBuildingGroup(theBuildingGroupName)->getParentName();
}

const int V3::BuildingGroups::getInfrastructureCost(const std::string& theBuildingGroupName) const
{
	return buildingGroups.at(theBuildingGroupName)->getInfrastructureCost();
}

const int V3::BuildingGroups::safeGetInfrastructureCost(const std::string& theBuildingGroupName) const
{
	return safeGetBuildingGroup(theBuildingGroupName)->getInfrastructureCost();
}

const std::shared_ptr<V3::BuildingGroup> V3::BuildingGroups::safeGetBuildingGroup(const std::string& theBuildingGroupName) const
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