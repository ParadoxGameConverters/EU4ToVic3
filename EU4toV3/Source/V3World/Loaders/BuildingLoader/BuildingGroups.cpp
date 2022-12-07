#include "BuildingGroups.h"
#include "BuildingGroup.h"
#include "Log.h"
#include <ranges>

void V3::BuildingGroups::setInfrastructureCosts()
{
	for (const auto& buildingGroup: std::views::values(buildingGroups))
	{
		std::string parentGroupName = buildingGroup->getParentName();
		while (buildingGroup->getInfrastructureCost() == 0 && !parentGroupName.empty())
		{
			buildingGroup->setInfrastructureCost(safeGetInfrastructureCost(parentGroupName));
			parentGroupName = safeGetParentName(parentGroupName);
		}
	}
}

void V3::BuildingGroups::addBuildingGroup(std::shared_ptr<BuildingGroup> theBuildingGroup)
{
	buildingGroups[theBuildingGroup->getName()] = theBuildingGroup;
}

std::optional<std::string> V3::BuildingGroups::getParentName(const std::string& theBuildingGroupName) const
{
	const auto& possibleBuildingGroup = buildingGroups.find(theBuildingGroupName);
	if (possibleBuildingGroup != buildingGroups.end())
	{
		return possibleBuildingGroup->second->getParentName();
	}
	else
	{
		return std::nullopt;
	}
}

const std::string& V3::BuildingGroups::safeGetParentName(const std::string& theBuildingGroupName) const
{
	return safeGetBuildingGroup(theBuildingGroupName)->getParentName();
}

std::optional<int> V3::BuildingGroups::getInfrastructureCost(const std::string& theBuildingGroupName) const
{
	const auto& possibleBuildingGroup = buildingGroups.find(theBuildingGroupName);
	if (possibleBuildingGroup != buildingGroups.end())
	{
		return possibleBuildingGroup->second->getInfrastructureCost();
	}
	else
	{
		return std::nullopt;
	}
}

int V3::BuildingGroups::safeGetInfrastructureCost(const std::string& theBuildingGroupName) const
{
	return safeGetBuildingGroup(theBuildingGroupName)->getInfrastructureCost();
}

std::shared_ptr<V3::BuildingGroup> V3::BuildingGroups::safeGetBuildingGroup(const std::string& theBuildingGroupName) const
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